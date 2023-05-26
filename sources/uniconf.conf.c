#include "uniconf.internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <libconfig.h>

static int uniconf__to_json(cJSON *node, config_setting_t *tree);
static int uniconf__set_number(cJSON *node, const char *name, double value);
static int uniconf__set_string(cJSON *node, const char *name, const char *value);
static int uniconf__set_aggregate(cJSON *node, const char *name, config_setting_t *tree);
/**
 * Parse the .conf file
 *
 * @param root
 * @param filepath
 * @param branch
 *
 * @return int
 */
int uniconf_conf(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    cJSON *node = uniconf_node(root, branch);

    if (node)
    {
        config_t the_config;
        config_init(&the_config);

        count = (CONFIG_FALSE == config_read_file(&the_config, filepath)) ? -(config_error_line(&the_config)) : uniconf__to_json(node, config_root_setting(&the_config));

        config_destroy(&the_config);
    }

    return count;
}

static int uniconf__to_json(cJSON *node, config_setting_t *tree)
{
    int count = -1;
    if (node && tree)
    {
        switch (config_setting_type(tree))
        {
        case CONFIG_TYPE_BOOL:
            count = uniconf__set_number(node, config_setting_name(tree), (double)config_setting_get_bool(tree));
            break;
        case CONFIG_TYPE_INT:
            count = uniconf__set_number(node, config_setting_name(tree), (double)config_setting_get_int(tree));
            break;
        case CONFIG_TYPE_INT64:
            count = uniconf__set_number(node, config_setting_name(tree), (double)config_setting_get_int64(tree));
            break;
        case CONFIG_TYPE_FLOAT:
            count = uniconf__set_number(node, config_setting_name(tree), config_setting_get_float(tree));
            break;
        case CONFIG_TYPE_STRING:
            count = uniconf__set_string(node, config_setting_name(tree), config_setting_get_string(tree));
            break;
        case CONFIG_TYPE_ARRAY:
        case CONFIG_TYPE_LIST:
        case CONFIG_TYPE_GROUP:
            count = uniconf__set_aggregate(node, config_setting_name(tree), tree);
            break;
        }
    }
    else
    {
        count = -2;
    }

    return count;
}

static int uniconf__set_number(cJSON *node, const char *name, double value)
{
    int count = -1;
    if (node)
    {
        if (cJSON_IsObject(node) && name)
        {
            cJSON_AddNumberToObject(node, name, value);
            count = 1;
        }
        else if (cJSON_IsArray(node))
        {
            cJSON *item = cJSON_CreateNumber(value);
            if (!cJSON_AddItemToArray(node, item))
            {
                cJSON_Delete(item);
                count = 0;
            }
            else
            {
                count = 1;
            }
        }
    }
    return count;
}

static int uniconf__set_string(cJSON *node, const char *name, const char *value)
{
    int count = -1;
    if (node)
    {
        char *expanded = uniconf_substitute(value);
        if (expanded)
        {
            if (cJSON_IsObject(node) && name)
            {
                cJSON_AddStringToObject(node, name, expanded);
                count = 1;
            }
            else if (cJSON_IsArray(node))
            {
                cJSON *item = cJSON_CreateString(expanded);
                if (!cJSON_AddItemToArray(node, item))
                {
                    cJSON_Delete(item);
                    count = 0;
                }
                else
                {
                    count = 1;
                }
            }
            free(expanded);
        }
    }
    return count;
}

static int uniconf__set_aggregate(cJSON *node, const char *name, config_setting_t *tree)
{
    int count = -1;
    if (node)
    {
        count = 0;
        if (cJSON_IsObject(node) && name)
        {
            node = cJSON_AddObjectToObject(node, name);
        }

        if (node)
        {
            int len = config_setting_length(tree);
            for (int i = 0; i < len; i++)
            {
                int ret = uniconf__to_json(node, config_setting_get_elem(tree, i));
                if (ret < 0)
                {
                    return -2;
                }
                count += ret;
            }
        }
    }

    return count;
}
