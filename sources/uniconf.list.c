#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/**
 * Parse the .list file
 *
 * @param root
 * @param filepath
 * @param branch
 *
 * @return int
 */
int uniconf_list(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    cJSON *node = uniconf_node(root, branch);
    if (node)
    {
        if (!cJSON_IsArray(node) && branch)
        {
            node = cJSON_CreateArray();
            cJSON_ReplaceItemInObjectCaseSensitive(root,branch,node);
        }
        if (cJSON_IsArray(node))
        {
            uniconf_FileByLine(filepath, line)
            {
                char *value = uniconf_trim(line, "\r\n");
                cJSON *item = cJSON_CreateString(value);
                if (item)
                {
                    if (cJSON_AddItemToArray(node, item))
                    {
                        count++;
                    }
                    else
                    {
                        cJSON_Delete(item);
                    }
                }
            }
            uniconf_EndByLine(line);
        }
    }

    return count;
}
