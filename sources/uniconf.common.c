#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/**
 * Build the correct path
 *
 * @param path
 * @param name
 *
 * @return allocated char* | NULL
 */
char *uniconf_makepath(const char *path, const char *name)
{
    char *result = NULL;

    if (name && strlen(name) > 0)
    {
        if (path && strlen(path) > 0)
        {
            asprintf(&result, "%s/%s", path, name);
        }
        else
        {
            result = strdup(name);
        }
    }
    else if (path && strlen(path) > 0)
    {
        result = strdup(path);
    }

    return result;
}

/**
 * Check the path
 *
 * @param path
 * @param name
 *
 * @return <0 = not found|error, 1 = directory, 0 = file
 */
int uniconf_check(const char *path, const char *name)
{
    struct stat path_stat;

    char *filename = uniconf_makepath(path, name);
    if (filename)
    {
        int result = stat(filename, &path_stat);
        free(filename);
        return result ? -errno
                      : S_ISDIR(path_stat.st_mode);
    }

    return -EINVAL; // Invalid argument
}

/**
 * Create|get the named node
 *
 * @param root
 * @param name
 * @return cJSON*
 */
cJSON *uniconf_node(cJSON *root, const char *name)
{
    cJSON *node = root;

    if (name && strlen(name) > 0)
    {
        node = cJSON_GetObjectItemCaseSensitive(root, name);
        if (!node)
        {
            node = cJSON_AddObjectToObject(root, name);
        }
    }

    return node;
}

/**
 * Is comment ?
 *
 * @param line
 * @param prefix
 * @return int
 */
int uniconf_is_commented(char *line, const char *prefix)
{
    if (line && strlen(line) > 0 && prefix && strlen(prefix) > 0)
    {
        char *ptr = line;
        while (isspace(*ptr))
        {
            ptr++;
        }
        return (strstr(ptr, prefix) == ptr);
    }
    return 0;
}

/**
 * Makes the trimmed string without trailing comments.
 * Modifies the string!
 *
 * @param str
 * @param trail
 *
 * @return char*
 */
char *uniconf_trim(char *str, char *trail)
{
    if (str)
    {
        while (isspace(*str))
        {
            str++;
        }

        char *ptr = strstr(str, trail);
        if (ptr)
        {
            *ptr = '\0';
        }
        else
        {
            ptr = str + strlen(str);
        }
        --ptr;
        while ((ptr > str) && isspace(*ptr)) //
        {
            *ptr = '\0';
            ptr--;
        }
    }
    return str;
}

/**
 * Returns pointer on the unquoted string.
 * Modifies the string!
 *
 * @param str
 *
 * @return char*
 */
char *uniconf_unquote(char *str)
{
    char *ptr = str;
    if (strchr("'\"", *ptr))
    {
        char quote = *ptr;
        int len = strlen(str);
        if (len > 1 && (quote == str[len - 1]))
        {
            str[len - 1] = '\0';
            return ptr + 1;
        }
    }
    return ptr;
}

/**
 * Find the variable in the tree
 *
 * @param root
 * @param varname
 *
 * @return cJSON*
 */
cJSON *uniconf_vardata(cJSON *root, char *varname)
{
    cJSON *var = root ? root : uniconf_get_root();
    if (varname)
    {
        for (char *sptr, *token = strtok_r(varname, PATH_DELIM, &sptr); var && token; token = strtok_r(NULL, PATH_DELIM, &sptr))
        {
            var = cJSON_GetObjectItemCaseSensitive(var, (const char *)token);
        }
    }
    return var;
}

/**
 * Try substitute named vars in the string.
 * Must be freed!
 *
 * @param root
 * @param str
 *
 * @return char*
 */
char *uniconf_substitute(cJSON *root, const char *str)
{
    char *result = NULL;
    if (str)
    {
        char *buffer = strdup(str);
        char *pointer = buffer;
        while (strchr(pointer, '$'))
        {
            // get prefix
            char *prefix = NULL;
            int pfxlen = 0;
            sscanf(pointer, "%m[^$]%n", &prefix, &pfxlen);
            if (prefix)
            {
                asprintf(&result, "%s%s", result ? result : "", prefix);
                FREE_AND_NULL(prefix);
            }
            pointer += pfxlen; // on $

            // get var
            char lbr = '\0';
            char rbr = '\0';
            char *varname = NULL;
            int len = 0;
            sscanf(pointer, "$%c%m[^])>}]%c%n", &lbr, &varname, &rbr, &len);

            cJSON *var = NULL;
            switch (lbr)
            {
            case '(':
                var = (')' == rbr) ? uniconf_vardata(root, varname) : NULL;
                break;
            case '[':
                var = (']' == rbr) ? uniconf_vardata(root, varname) : NULL;
                break;
            case '{':
                var = ('}' == rbr) ? uniconf_vardata(root, varname) : NULL;
                break;
            case '<':
                var = ('>' == rbr) ? uniconf_vardata(root, varname) : NULL;
                break;
            }
            FREE_AND_NULL(varname);

            if (var)
            {
                if (cJSON_IsString(var))
                {
                    asprintf(&result, "%s%s", result ? result : "", var->valuestring);
                }
                else if (cJSON_IsNumber(var))
                {
                    asprintf(&result, "%s%.0f", result ? result : "", var->valuedouble);
                }
                pointer += len;
            }
            else
            {
                break;
            }
        }
        asprintf(&result, "%s%s", result ? result : "", pointer);
        free(buffer);
    }
    return result;
}

/**
 * Set/replace named string
 *
 * @param node
 * @param name
 * @param value
 *
 * @return int
 */
int uniconf_set(cJSON *node, char *name, char *value)
{
    if (node && name && value)
    {
        if (cJSON_GetObjectItemCaseSensitive(node, name))
        {
            cJSON_DeleteItemFromObject(node, name);
        }
        if (cJSON_AddStringToObject(node, name, value))
        {
            return 1;
        }
    }
    return 0;
}
