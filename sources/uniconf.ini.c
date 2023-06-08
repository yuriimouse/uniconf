#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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
    if (node)
    {
        uniconf_FileByLine(filepath, line)
        {
            if (!uniconf_is_commented(line, "#"))
            {
                if ('[' == line[0])
                {
                    if (strchr(line, ']'))
                    {
                        node = uniconf_node(node, uniconf_trim(line, "]") + 1);
                    }
                    else
                    {
                        uniconf_error_file(filepath, _lineno, "section name error");
                    }
                }
                else
                {
                    char *tok_ptr = NULL;
                    char *name = strtok_r(line, " =", &tok_ptr);
                    if (name)
                    {
                        char *value = strtok_r(NULL, "\n", &tok_ptr);

                        value = uniconf_trim(uniconf_trim(value, "//"), "##");
                        char *expanded = uniconf_substitute(NULL, value);
                        if (expanded)
                        {
                            count += uniconf_set(node, name, uniconf_unquote(expanded));
                            free(expanded);
                        }
                    }
                }
            }
        }
        uniconf_EndByLine(line);
    }

    return count;
}
