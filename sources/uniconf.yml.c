#include "uniconf.internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <lists.h>

struct yaml_level
{
    int prefix;
    cJSON *node;
};

static cJSON *uniconf_yml__toArray(cJSON *node, char *value)
{
    cJSON *ret = NULL;
    if (cJSON_IsNull(node))
    {
        node->type = cJSON_Array;
    }
    if (cJSON_IsArray(node))
    {
        char *expanded = uniconf_substitute(uniconf_trim(value + 1, "#"));
        ret = cJSON_CreateString(expanded);
        cJSON_AddItemToArray(node, ret);
        free(expanded);
    }
    return ret;
}

static cJSON *uniconf_yml__toObject(cJSON *node, char *value)
{
    if (cJSON_IsNull(node))
    {
        node->type = cJSON_Object;
    }
    if (cJSON_IsObject(node))
    {
        return cJSON_AddNullToObject(node, uniconf_trim(value, ":"));
    }
    return NULL;
}

static cJSON *uniconf_yml__append(cJSON *node, char *value)
{
    if (node)
    {
        cJSON *ret = NULL;
        if ('-' == value[0])
        {
            ret = uniconf_yml__toArray(node, value);
        }
        else if (strchr(value, ':'))
        {
            ret = uniconf_yml__toObject(node, value);
        }
        return ret;
    }
    return NULL;
}

/**
 * Parse the .yml file
 *
 * @param root
 * @param filepath
 * @param branch
 * @return int
 */
int uniconf_yml(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    cJSON *node = uniconf_node(root, branch);
    if (node)
    {
        list_t *stack = list_construct();
        struct yaml_level *level = NULL;

        uniconf_FileByLine(filepath, line)
        {
            if (!uniconf_is_commented(line, "#"))
            {
                char *prefix = NULL;
                int nnn = 0;
                char *value = NULL;
                sscanf(line, "%m[ ]%n", &prefix, &nnn);
                sscanf(line + nnn, "%m[^#]", &value);

                level = list_get(stack);
                if (level && nnn <= level->prefix)
                {
                    while (level && nnn < level->prefix)
                    {
                        free(list_pop(stack));
                        level = list_get(stack);
                    }

                    if (level && nnn == level->prefix)
                    {
                        uniconf_yml__append(level->node, value);
                        count++;
                    }
                    else
                    {
                        level = malloc(sizeof(struct yaml_level));
                        level->prefix = nnn;
                        level->node = uniconf_yml__append(node, value);
                        list_push(stack, level);
                        count++;
                    }
                }
                else
                {
                    level = malloc(sizeof(struct yaml_level));
                    level->prefix = nnn;
                    level->node = uniconf_yml__append(node, value);
                    list_push(stack, level);
                    count++;
                }
                FREE_AND_NULL(prefix);
                FREE_AND_NULL(value);
            }
        }
        uniconf_EndByLine(line);
        list_destruct(stack,NULL);
    }

    return count;
}
