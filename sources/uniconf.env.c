#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/**
 * Parse the .env file
 *
 * Removes quotes, if any.
 * Each $() variable will be replaced.
 * Trailing comments start with ###
 *
 * @param root
 * @param filepath
 * @param branch
 *
 * @return int
 */
int uniconf_env(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    cJSON *node = uniconf_node(root, branch);
    if (node)
    {
        uniconf_FileByLine(filepath, line)
        {
            if (!uniconf_is_commented(line, "#"))
            {
                char *name = NULL;
                char *value = NULL;

                sscanf(line, "%ms = %m[^\n]", &name, &value);
                if (name && value)
                {
                    value = uniconf_trim(value, "###");
                    char *expanded = uniconf_substitute(NULL, value);
                    if (expanded)
                    {
                        count += uniconf_set(node, name, uniconf_unquote(expanded));
                        free(expanded);
                    }
                }
                FREE_AND_NULL(name);
                FREE_AND_NULL(value);
            }
        }
        uniconf_EndByLine(line);
    }

    return count;
}
