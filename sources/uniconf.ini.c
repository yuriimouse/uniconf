#include "uniconf.internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

/**
 * Parse the .ini file
 *
 * Each $() variable in the "" will be replaced.
 * Removes quotes, if any.
 * Trailing comments start with // or ##
 *
 * @param root
 * @param filepath
 * @param branch
 * @return int
 */
int uniconf_ini(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    cJSON *node = uniconf_node(root, branch);
    cJSON *section = NULL;
    if (node)
    {
        uniconf_FileByLine(filepath, line)
        {
            if (!uniconf_is_commented(line, "#"))
            {
                if ('[' == line[0])
                {
                    section = uniconf_node(node, uniconf_trim(line, "]") + 1);
                }
                else
                {
                    char *tok_ptr = NULL;
                    char *name = strtok_r(line, " =", &tok_ptr);
                    char *value = strtok_r(NULL, "\n", &tok_ptr);

                    value = uniconf_trim(uniconf_trim(value, "//"), "##");
                    if ('"' == value[0])
                    {
                        char *expanded = uniconf_substitute(value);
                        if (expanded)
                        {
                            count += uniconf_set(section ? section : node, name, uniconf_unquote(expanded));
                            free(expanded);
                        }
                    }
                    else
                    {
                        count += uniconf_set(section ? section : node, name, uniconf_unquote(value));
                    }
                }
            }
        }
        uniconf_EndByLine(line);
    }

    return count;
}
