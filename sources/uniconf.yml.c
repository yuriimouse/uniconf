#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <lists.h>
#include <yaml.h>

static void process_event(cJSON *json, yaml_event_t *event);
static cJSON *node = NULL;

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
    node = uniconf_nodeNULL(root, branch);

    if (node)
    {
        FILE *_file = NULL;
        if (filepath && (_file = fopen(filepath, "rt")))
        {
            yaml_parser_t parser;
            yaml_event_t event;
            yaml_event_type_t event_type;

            yaml_parser_initialize(&parser);
            yaml_parser_set_input_file(&parser, _file);

            do
            {
                if (!yaml_parser_parse(&parser, &event))
                {
                    uniconf_error("Failed to parse file '%s': '%s'", filepath, parser.problem);
                    break;
                }
                process_event(node, &event);
                event_type = event.type;
                yaml_event_delete(&event);
            } while (event_type != YAML_STREAM_END_EVENT);

            yaml_parser_delete(&parser);
            fclose(_file);
        }
        else
        {
            uniconf_error("Failed to open file '%s'", filepath);
        }
    }
    return count;
}

#define STRVAL(x) ((x) ? (char *)(x) : "")
static list_t *stack = NULL;

static char *astrncpy(char *src, int len)
{
    char *str = malloc(len + 1);
    if (str)
    {
        strncpy(str, src, len);
        str[len] = '\0';
    }
    return str;
}

static cJSON *add_NullToArray(cJSON *json)
{
    cJSON *item = cJSON_CreateNull();
    if (!cJSON_AddItemToArray(json, item))
    {
        cJSON_Delete(item);
        item = NULL;
    }
    return item;
}

static cJSON *add_ObjectToArray(cJSON *json)
{
    cJSON *item = cJSON_CreateObject();
    if (!cJSON_AddItemToArray(json, item))
    {
        cJSON_Delete(item);
        item = NULL;
    }
    return item;
}

static cJSON *add_StringToArray(cJSON *json, char *name, int namelen)
{
    cJSON *item = NULL;
    char *buff = astrncpy(name, namelen);
    if (buff)
    {
        char *expanded = uniconf_substitute(NULL, buff);
        if (expanded)
        {
            item = cJSON_CreateString(expanded);
            if (item && !cJSON_AddItemToArray(json, item))
            {
                cJSON_Delete(item);
                item = NULL;
            }
            free(expanded);
        }
        free(buff);
    }
    return item;
}

static cJSON *add_NullToObject(cJSON *json, char *name, int namelen)
{
    cJSON *item = NULL;
    char *buff = astrncpy(name, namelen);
    if (buff)
    {
        item = cJSON_CreateNull();
        if (item && !cJSON_AddItemToObject(json, buff, item))
        {
            cJSON_Delete(item);
            item = NULL;
        }
        free(buff);
    }
    return item;
}

static void set_AsString(cJSON *json, char *value, int valuelen)
{
    if (cJSON_IsNull(json))
    {
        char *buff = astrncpy(value, valuelen);
        if (buff)
        {
            char *expanded = uniconf_substitute(NULL, buff);
            if (expanded)
            {
                json->type = cJSON_String;
                json->valuestring = expanded;
            }
            free(buff);
        }
    }
}

static void set_AsArray(cJSON *json)
{
    if (cJSON_IsNull(json))
    {
        json->type = cJSON_Array;
    }
}

static void set_AsObject(cJSON *json)
{
    if (cJSON_IsNull(json))
    {
        json->type = cJSON_Object;
    }
}

static void process_event(cJSON *json, yaml_event_t *event)
{
    switch (event->type)
    {
    case YAML_DOCUMENT_START_EVENT:
        stack = list_construct();
        list_push(stack, json);
        break;
    case YAML_DOCUMENT_END_EVENT:
        stack = list_destruct(stack, NULL);
        break;
    case YAML_SCALAR_EVENT:
        // JSONise
        {
            cJSON *item = (cJSON *)list_get(stack);
            if (cJSON_IsNull(item))
            {
                set_AsString(item, STRVAL(event->data.scalar.value), (int)event->data.scalar.length);
                list_pop(stack);
            }
            else if (cJSON_IsObject(item))
            {
                list_push(stack, add_NullToObject(item, STRVAL(event->data.scalar.value), (int)event->data.scalar.length));
            }
            else if (cJSON_IsArray(item))
            {
                add_StringToArray(item, STRVAL(event->data.scalar.value), (int)event->data.scalar.length);
            }
        }
        break;
    case YAML_SEQUENCE_START_EVENT:
        // JSONise
        {
            cJSON *item = (cJSON *)list_get(stack);
            if (cJSON_IsNull(item))
            {
                set_AsArray(item);
            }
            else if (cJSON_IsArray(item))
            {
                list_push(stack, add_NullToArray(item));
            }
        }
        break;
    case YAML_SEQUENCE_END_EVENT:
        list_pop(stack);
        break;
    case YAML_MAPPING_START_EVENT:
        // JSONise
        {
            cJSON *item = (cJSON *)list_get(stack);
            if (cJSON_IsNull(item))
            {
                set_AsObject(item);
            }
            else if (cJSON_IsArray(item))
            {
                list_push(stack, add_ObjectToArray(item));
            }
        }
        break;
    case YAML_MAPPING_END_EVENT:
        list_pop(stack);
        break;
    default:
        break;
    }
}
