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
        int errNo = errno;
        free(filename);
        return result ? -errNo
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

    if (name && *name)
    {
        node = cJSON_GetObjectItemCaseSensitive(root, name);
        if (!node)
        {
            node = cJSON_AddObjectToObject(root, name);
            // node = cJSON_AddNullToObject(root, name);
        }
    }

    return node;
}

/**
 * Create|get the NULL named node
 *
 * @param root
 * @param name
 * @return cJSON*
 */
cJSON *uniconf_nodeNULL(cJSON *root, const char *name)
{
    cJSON *node = root;

    if (name && *name)
    {
        node = cJSON_GetObjectItemCaseSensitive(root, name);
        if (!node)
        {
            node = cJSON_AddNullToObject(root, name);
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
 * Cut trailing spaces
 *
 * @param str
 * @return char* last nonspaced char
 */
static char *uniconf__backSpaces(char *str)
{
    if (str)
    {
        while (('\0' == *str) || isspace(*str))
        {
            *str = '\0';
            str--;
        }
    }
    return str;
}

/**
 * Trims trailing comments and spaces
 *
 * @param tail
 * @param trailer
 * @return char* last significant char
 */
static char *uniconf__cutOff(char *tail, char *trailer)
{
    if (trailer && *trailer)
    {
        char *ptr = strstr(tail, trailer);
        if (ptr)
        {
            *ptr = 0;
            return uniconf__backSpaces(ptr);
        }
    }
    return uniconf__backSpaces(strchr(tail, '\0'));
}

/**
 * Extracts a string with/without quotes and trims trailing spaces and comments
 * Modifies the string!
 *
 * @param str
 * @param trail
 * @return char*
 */
char *uniconf_string(char *str, char *trail)
{
    if (str && *str)
    {

        // trim EOL
        char *ret = strtok(str, "\r\n");

        // trim leading spaces
        while (isspace(*ret))
        {
            ret++;
        }

        // check if quoted
        char quoted = '\0';
        if (strchr("'\"`", ret[0]))
        {
            quoted = ret[0];
        }

        if (quoted)
        {
            // find the closing quote
            char *ptr = NULL;
            for (ptr = ret + 1; *ptr && (*ptr != quoted); ptr++)
            {
                if ('\\' == *ptr)
                {
                    ptr++;
                }
            }
            // cut the correct trailer after closed
            if (*ptr == quoted)
            {
                if (uniconf__cutOff(ptr + 1, trail) == ptr)
                {
                    return ret;
                }
            }
            return NULL;
        }
        else
        {
            uniconf__cutOff(ret, trail);
        }
        return ret;
    }
    return NULL;
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
    if (strchr("'\"`", *ptr))
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
                char *temp = NULL;
                asprintf(&temp, "%s%s", result ? result : "", prefix);
                if (result)
                    free(result);
                result = temp;
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

            if (var)
            {
                if (cJSON_IsString(var))
                {
                    char *temp = NULL;
                    asprintf(&temp, "%s%s", result ? result : "", var->valuestring);
                    if (result)
                        free(result);
                    result = temp;
                }
                else if (cJSON_IsNumber(var))
                {
                    char *temp = NULL;
                    asprintf(&temp, "%s%.0f", result ? result : "", var->valuedouble);
                    if (result)
                        free(result);
                    result = temp;
                }
                pointer += len;
            }
            else
            {
                uniconf_error("WARNING: variable '%s' is undefined", varname);
                FREE_AND_NULL(varname);
                break;
            }
            FREE_AND_NULL(varname);
        }
        char *temp = NULL;
        asprintf(&temp, "%s%s", result ? result : "", pointer);
        if (result)
            free(result);
        result = temp;
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
        cJSON *item = cJSON_CreateString(value);
        if (item)
        {
            if (cJSON_AddItemToObject(node, name, item))
            {
                return 1;
            }
            cJSON_Delete(item);
        }
    }
    return 0;
}
