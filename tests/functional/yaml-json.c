/*
 * Example libyaml parser.
 *
 * This is a simple libyaml parser example which scans and prints
 * the libyaml parser events.
 *
 */
#define _GNU_SOURCE

#include <cjson/cJSON.h>
#include <lists.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define INDENT "  "
#define STRVAL(x) ((x) ? (char *)(x) : "")

static void indent(int level)
{
    int i;
    for (i = 0; i < level; i++)
    {
        printf("%s", INDENT);
    }
}

static cJSON *json = NULL;
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
        item = cJSON_CreateString(buff);
        if (item && !cJSON_AddItemToArray(json, item))
        {
            cJSON_Delete(item);
            item = NULL;
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
            json->type = cJSON_String;
            json->valuestring = buff;
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

static void print_event(yaml_event_t *event)
{
    static int level = 0;

    switch (event->type)
    {
    case YAML_NO_EVENT:
        indent(level);
        printf("no-event (%d)\n", event->type);
        break;
    case YAML_STREAM_START_EVENT:
        indent(level++);
        printf("stream-start-event (%d)\n", event->type);
        break;
    case YAML_STREAM_END_EVENT:
        indent(--level);
        printf("stream-end-event (%d)\n", event->type);
        break;
    case YAML_DOCUMENT_START_EVENT:
        indent(level++);
        printf("document-start-event (%d)\n", event->type);
        if (!json)
        {
            printf("JSONISE\n");
            stack = list_construct();
            json = cJSON_CreateNull();
            list_push(stack, json);
        }
        else
        {
            printf("OOPS\n");
        }
        break;
    case YAML_DOCUMENT_END_EVENT:
        indent(--level);
        printf("document-end-event (%d)\n", event->type);
        if (json)
        {
            char *str = cJSON_Print(json);
            printf("=======\n%s\n", str);
            cJSON_Delete(json);
            json = NULL;
            free(str);
            stack = list_destruct(stack, NULL);
        }
        break;
    case YAML_ALIAS_EVENT:
        indent(level);
        printf("alias-event (%d)\n", event->type);
        break;
    case YAML_SCALAR_EVENT:
        indent(level);
        printf("scalar-event (%d) = {value=\"%s\", length=%d} ",
               event->type,
               STRVAL(event->data.scalar.value),
               (int)event->data.scalar.length);
        // JSONise
        {
            cJSON *item = (cJSON *)list_get(stack);
            if (cJSON_IsNull(item))
            {
                set_AsString(item, STRVAL(event->data.scalar.value), (int)event->data.scalar.length);
                list_pop(stack);
                printf("set_AsString");
            }
            else if (cJSON_IsObject(item))
            {
                list_push(stack, add_NullToObject(item, STRVAL(event->data.scalar.value), (int)event->data.scalar.length));
                printf("add_NullToObject, push");
            }
            else if (cJSON_IsArray(item))
            {
                add_StringToArray(item, STRVAL(event->data.scalar.value), (int)event->data.scalar.length);
                printf("add_StringToArray");
            }
        }
        printf("\n");
        break;
    case YAML_SEQUENCE_START_EVENT:
        indent(level++);
        printf("sequence-start-event (%d) ", event->type);
        // JSONise
        {
            cJSON *item = (cJSON *)list_get(stack);
            if (cJSON_IsNull(item))
            {
                set_AsArray(item);
                // list_push(stack, add_NullToArray(item));
                printf("set_AsArray");
            }
            else if (cJSON_IsObject(item))
            {
                printf("YAML_SEQUENCE_START_EVENT error\n");
            }
            else if (cJSON_IsArray(item))
            {
                list_push(stack, add_NullToArray(item));
                printf("add_NullToArray, push");
            }
        }
        printf("\n");
        break;
    case YAML_SEQUENCE_END_EVENT:
        indent(--level);
        printf("sequence-end-event (%d) pop\n", event->type);
        // JSONise
        {
            list_pop(stack);
        }
        break;
    case YAML_MAPPING_START_EVENT:
        indent(level++);
        printf("mapping-start-event (%d) ", event->type);
        // JSONise
        {
            cJSON *item = (cJSON *)list_get(stack);
            if (cJSON_IsNull(item))
            {
                set_AsObject(item);
                printf("set_AsObject");
            }
            else if (cJSON_IsObject(item))
            {
                printf("YAML_MAPPING_START_EVENT error\n");
            }
            else if (cJSON_IsArray(item))
            {
                list_push(stack, add_ObjectToArray(item));
                printf("add_ObjectToArray, push");
            }
        }
        printf("\n");
        break;
    case YAML_MAPPING_END_EVENT:
        indent(--level);
        printf("mapping-end-event (%d) pop\n", event->type);
        // JSONise
        {
            list_pop(stack);
        }
        break;
    }
    if (level < 0)
    {
        fprintf(stderr, "indentation underflow!\n");
        level = 0;
    }
}

int main(int argc, char *argv[])
{
    yaml_parser_t parser;
    yaml_event_t event;
    yaml_event_type_t event_type;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, stdin);

    do
    {
        if (!yaml_parser_parse(&parser, &event))
            goto error;
        print_event(&event);
        event_type = event.type;
        yaml_event_delete(&event);
    } while (event_type != YAML_STREAM_END_EVENT);

    yaml_parser_delete(&parser);
    return EXIT_SUCCESS;

error:
    fprintf(stderr, "Failed to parse: %s\n", parser.problem);
    yaml_parser_delete(&parser);
    return EXIT_FAILURE;
}
