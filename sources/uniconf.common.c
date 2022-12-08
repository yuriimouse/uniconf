#include "uniconf.h"

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
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
            ptr[0] = '\0';
        }
        else
        {
            ptr = str + strlen(str);
        }
        while (ptr > str && isspace(*ptr))
        {
            ptr[0] = '\0';
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
 * Try substitute named vars in the string.
 * Must be freed!
 *
 * @param str
 *
 * @return char*
 */
char *uniconf_substitute(const char *str)
{
    char *result = NULL;
    if (str)
    {

        char *buffer = strdup(str);
        int skip = 0;
        while (strchr(buffer + skip, '$'))
        {
            char *prefix = NULL;
            // copy prefix
            sscanf(buffer + skip, "%m[^$]", &prefix);

            if (prefix)
            {
                asprintf(&result, "%s%s", result ? result : "", prefix);
                skip += strlen(prefix);
            }

            skip++;
            // find var name
            char rbracket = '\0';
            switch (*(buffer + skip))
            {
            case '(':
                rbracket = ')';
                break;
            case '[':
                rbracket = ']';
                break;
            case '{':
                rbracket = '}';
                break;
            case '<':
                rbracket = '>';
                break;
            default:
                rbracket = '\0';
            }

            char *ptr = strchr(buffer + skip, rbracket);
            char *var = uniconf_get_vardata(buffer + skip, ptr ? ptr - buffer - skip : 0, "");
            if (!rbracket || !var)
            {
                asprintf(&result, "%s$%s", result ? result : "", buffer + skip);
                skip = strlen(buffer);
            }
            else
            {
                asprintf(&result, "%s%s", result ? result : "", var);
                skip = ptr - buffer + 1;
            }
            if (var)
            {
                free(var);
                var = NULL;
            }
        }
        asprintf(&result, "%s%s", result ? result : "", buffer + skip);
        free(buffer);
    }
    return result;
}

/**
 * Find the variable in the tree
 *
 * @param varname
 *
 * @return cJSON*
 */
cJSON *uniconf_vardata(char *varname)
{
    cJSON *var = uniconf_get_root();
    if (varname)
    {
        for (char *sptr, *token = strtok_r(varname, "/.", &sptr); var && token; token = strtok_r(NULL, "/.", &sptr))
        {
            var = cJSON_GetObjectItemCaseSensitive(var, (const char *)token);
        }
    }
    return var;
}

/**
 * Gets the named var from the json structure
 *
 * @param str
 * @param len
 * @param not_found
 * @param ...
 *
 * @return char*
 */
char *uniconf_get_vardata(char *str, int len, char *not_found, ...)
{
    if (!str || !len)
    {
        return NULL;
    }

    char *varname = NULL;
    asprintf(&varname, "%.*s", len - 2, str + 1);

    cJSON *var = uniconf_vardata(varname);
    free(varname);

    char *result = NULL;
    if (!var)
    {
        if (not_found)
        {
            va_list ap;
            va_start(ap, not_found);
            vasprintf(&result, not_found, ap);
            va_end(ap);
        }
    }
    else if (cJSON_IsString(var))
    {
        result = strdup(var->valuestring);
    }
    else if (cJSON_IsNumber(var))
    {
        asprintf(&result, "%.0f", var->valuedouble);
    }
    else
    {
        result = strdup("");
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