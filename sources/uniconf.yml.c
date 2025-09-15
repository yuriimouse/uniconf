#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

static int convert_yaml_to_cjson(
    yaml_parser_t *parser,
    yaml_document_t *document,
    int node_index,
    cJSON **out,
    const char *filepath)
{
    yaml_node_t *node = yaml_document_get_node(document, node_index);
    if (!node)
        return 0;

    int scalar_count = 0;

    switch (node->type)
    {
    case YAML_SCALAR_NODE:
    {
        const char *val = (const char *)node->data.scalar.value;
        cJSON *item = NULL;

        if (!val)
            val = "";

        // Try to detect basic types
        if (strcmp(val, "true") == 0 || strcmp(val, "True") == 0)
        {
            item = cJSON_CreateTrue();
        }
        else if (strcmp(val, "false") == 0 || strcmp(val, "False") == 0)
        {
            item = cJSON_CreateFalse();
        }
        else if (strcmp(val, "null") == 0 || strcmp(val, "Null") == 0 || strcmp(val, "~") == 0)
        {
            item = cJSON_CreateNull();
        }
        else
        {
            char *endptr = NULL;
            double num = strtod(val, &endptr);
            if (endptr && *endptr == '\0')
            {
                item = cJSON_CreateNumber(num);
            }
            else
            {
                item = cJSON_CreateString(val);
            }
        }

        if (!item)
        {
            uniconf_error_file(filepath, 0, "failed to create JSON scalar");
            return 0;
        }

        *out = item;
        return 1;
    }

    case YAML_SEQUENCE_NODE:
    {
        cJSON *array = cJSON_CreateArray();
        if (!array)
        {
            uniconf_error_file(filepath, 0, "failed to create JSON array");
            return 0;
        }

        for (yaml_node_item_t *i = node->data.sequence.items.start;
             i < node->data.sequence.items.top; i++)
        {

            cJSON *child = NULL;
            scalar_count += convert_yaml_to_cjson(parser, document, *i, &child, filepath);
            if (child)
                cJSON_AddItemToArray(array, child);
        }

        *out = array;
        return scalar_count;
    }

    case YAML_MAPPING_NODE:
    {
        cJSON *obj = cJSON_CreateObject();
        if (!obj)
        {
            uniconf_error_file(filepath, 0, "failed to create JSON object");
            return 0;
        }

        for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
             pair < node->data.mapping.pairs.top; pair++)
        {

            yaml_node_t *key_node = yaml_document_get_node(document, pair->key);
            if (!key_node || key_node->type != YAML_SCALAR_NODE)
                continue;

            const char *key = (const char *)key_node->data.scalar.value;
            if (!key)
                key = "";

            cJSON *child = NULL;
            scalar_count += convert_yaml_to_cjson(parser, document, pair->value, &child, filepath);
            if (child)
                cJSON_AddItemToObject(obj, key, child);
        }

        *out = obj;
        return scalar_count;
    }

    default:
        uniconf_error_file(filepath, 0, "unsupported YAML node type");
        return 0;
    }
}

int uniconf_yml(cJSON *root, const char *filepath, const char *branch)
{
    if (!root || !filepath)
    {
        uniconf_error("uniconf_yml: invalid arguments");
        return -1;
    }

    FILE *fh = fopen(filepath, "rb");
    if (!fh)
    {
        uniconf_error_file(filepath, 0, "failed to open YAML file");
        return -2;
    }

    yaml_parser_t parser;
    yaml_document_t document;

    if (!yaml_parser_initialize(&parser))
    {
        fclose(fh);
        uniconf_error("uniconf_yml: failed to initialize YAML parser");
        return -3;
    }

    yaml_parser_set_input_file(&parser, fh);

    if (!yaml_parser_load(&parser, &document))
    {
        yaml_parser_delete(&parser);
        fclose(fh);
        uniconf_error_file(filepath, 0, "failed to parse YAML");
        return -4;
    }

    yaml_node_t *root_node = yaml_document_get_root_node(&document);
    if (!root_node || root_node->type != YAML_MAPPING_NODE)
    {
        yaml_document_delete(&document);
        yaml_parser_delete(&parser);
        fclose(fh);
        uniconf_error_file(filepath, 0, "YAML root must be a mapping");
        return -5;
    }

    cJSON *target = root;
    if (branch && *branch)
    {
        cJSON *branch_obj = cJSON_GetObjectItem(root, branch);
        if (branch_obj && !cJSON_IsObject(branch_obj))
        {
            yaml_document_delete(&document);
            yaml_parser_delete(&parser);
            fclose(fh);
            uniconf_error_file(filepath, 0, "branch '%s' exists but is not an object", branch);
            return -6;
        }
        if (!branch_obj)
        {
            branch_obj = cJSON_CreateObject();
            if (!branch_obj)
            {
                yaml_document_delete(&document);
                yaml_parser_delete(&parser);
                fclose(fh);
                uniconf_error("uniconf_yml: failed to create JSON object for branch '%s'", branch);
                return -7;
            }
            cJSON_AddItemToObject(root, branch, branch_obj);
        }
        target = branch_obj;
    }

    int scalar_count = 0;
    for (yaml_node_pair_t *pair = root_node->data.mapping.pairs.start;
         pair < root_node->data.mapping.pairs.top; pair++)
    {

        yaml_node_t *key_node = yaml_document_get_node(&document, pair->key);
        if (!key_node || key_node->type != YAML_SCALAR_NODE)
            continue;

        const char *key = (const char *)key_node->data.scalar.value;
        if (!key)
            key = "";

        cJSON *val = NULL;
        scalar_count += convert_yaml_to_cjson(&parser, &document, pair->value, &val, filepath);
        if (val)
            cJSON_AddItemToObject(target, key, val);
    }

    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    fclose(fh);
    return scalar_count;
}
