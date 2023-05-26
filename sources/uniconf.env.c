#include "uniconf.internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

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
                char *tok_ptr = NULL;
                char *name = strtok_r(line, " =", &tok_ptr);
                char *value = strtok_r(NULL, "\n", &tok_ptr);

                value = uniconf_trim(value, "###");
                char *expanded = uniconf_substitute(value);
                if (expanded)
                {
                    count += uniconf_set(node, name, uniconf_unquote(expanded));
                    free(expanded);
                }
            }
            uniconf_EndByLine(line);
        }
    }

    return count;
}
