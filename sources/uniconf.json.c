#include "uniconf.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

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
    cJSON *node = uniconf_node(root, branch);
    FILE *file = NULL;

    if (node && filepath && (file = fopen(filepath, "rt")))
    {
        char *buffer = NULL;
        size_t len = 0;
        getdelim(&buffer, &len, '\0', file);
        fclose(file);

        cJSON *json = cJSON_Parse(buffer);
        if (json && cJSON_IsObject(json))
        {
            for (cJSON *element = json->child; element != NULL; element = element->next)
            {
                cJSON *dup = cJSON_Duplicate(element, 1);
                if (dup)
                {
                    if (cJSON_ReplaceItemInObjectCaseSensitive(node, element->string, dup))
                    {
                        count++;
                        uniconf_json_substitute(dup);
                    }
                    else
                    {
                        cJSON_Delete(dup);
                    }
                }
            }
        }
        cJSON_Delete(json);

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
        char *expanded = uniconf_substitute(json->valuestring);
        if (expanded)
        {
            cJSON_SetValuestring(json, expanded);
            free(expanded);
        }
    }
}
