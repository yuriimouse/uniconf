#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void uniconf_json_substitute(cJSON *json);

/**
 * Parse the .json file
 *
 * Each $() will be substituted
 *
 * @param root
 * @param filepath
 * @param branch
 * @return int
 */
int uniconf_json(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    // cJSON *node = uniconf_node(root, branch);
    FILE *file = NULL;

    if (root && filepath && (file = fopen(filepath, "rt")))
    {
        char *buffer = NULL;
        size_t len = 0;
        getdelim(&buffer, &len, '\0', file);
        fclose(file);

        cJSON *json = cJSON_Parse(buffer);
        if (!json)
        {
            uniconf_error_file(filepath, 0, "%s", cJSON_GetErrorPtr());
        }
        else if (branch && *branch)
        {
            // make branch
            cJSON_AddItemToObject(root, branch, json);
            uniconf_json_substitute(json);
            count++;
        }
        else if(cJSON_IsObject(json) || cJSON_IsArray(json))
        {
            // merge json to node
            for (cJSON *element = json->child; element != NULL; element = element->next)
            {
                if (cJSON_IsObject(root))
                {
                    cJSON_AddItemToObject(root, element->string, cJSON_Duplicate(element, 1));
                }
                else if (cJSON_IsArray(root))
                {
                    cJSON_AddItemToArray(root, cJSON_Duplicate(element, 1));
                    count++;
                }
            }
            uniconf_json_substitute(root);
            cJSON_Delete(json);
        }
        else
        {
            uniconf_error_file(filepath, 1, "not an object or array");
        }
        if (buffer)
        {
            free(buffer);
        }
    }

    return count;
}

static void uniconf_json_substitute(cJSON *json)
{
    if (cJSON_IsObject(json) || cJSON_IsArray(json))
    {
        for (cJSON *element = json->child; element != NULL; element = element->next)
        {
            uniconf_json_substitute(element);
        }
    }
    else if (cJSON_IsString(json))
    {
        char *expanded = uniconf_substitute(NULL, cJSON_GetStringValue(json));
        if (expanded)
        {
            cJSON_SetValuestring(json, expanded);
            free(expanded);
        }
    }
}
