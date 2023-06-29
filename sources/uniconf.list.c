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
    cJSON *node = cJSON_GetObjectItemCaseSensitive(root, branch);
    if (!node)
    {
        node = cJSON_AddArrayToObject(root, branch);
    }
    if (!cJSON_IsArray(node))
    {
        uniconf_error("ERROR: error type for file '%s' at branch '%s'", filepath, branch);
    }
    else
    {
        uniconf_FileByLine(filepath, line)
        {
            char *value = uniconf_unquote(uniconf_trim(line, "\r\n"));
            if (value)
            {
                if ((1 == _lineno) && ('[' == value[0]))
                { // nested array
                    cJSON *item = cJSON_CreateArray();
                    if (cJSON_AddItemToArray(node, item))
                    {
                        node = item;
                    }
                }
                else if (strchr("[]#", value[0]))
                {
                    continue;
                }
                else
                {
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
            }
        }
        uniconf_EndByLine(line);
    }

    return count;
}
