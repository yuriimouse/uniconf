#include "uniconf.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

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
                    count++;
                    if (!cJSON_AddItemToObject(node, element->string, dup))
                    {
                        count--;
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
