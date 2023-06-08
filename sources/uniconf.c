/**
 * The universal cJSON based configurator
 *
 * Acceptable formats:
 * .env
 * .ini
 * .conf
 * .json
 * .yml
 *
 * @author Yurii Prudius [https://github.com/yuriimouse]
 * @link https://github.com/yuriimouse/uniconf
 **/

#include "uniconf.h"
#include "uniconf.internal.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char *
    uniconf_path = NULL;
static uniconf_t
    uniconf_root = NULL;

/**
 * Get the root
 *
 * @return uniconf_t
 */
uniconf_t uniconf_get_root()
{
    return uniconf_root;
}

static int uniconf_process(uniconf_t node, const char *path, const char *name);
static int uniconf_dir(uniconf_t root, const char *path, const char *name);
static int uniconf_file(uniconf_t root, const char *path, const char *filename);
static uniconf_t uniconf_object_v(uniconf_t object, const char *format, va_list ap);

/**
 * Process the directory entry into the config node
 *
 * @param node
 * @param path
 * @param name
 *
 * @return <0 - error, >= 0 - count
 */
static int uniconf_process(uniconf_t node, const char *path, const char *name)
{
    if (name && ((0 == strcmp(".", name)) || (0 == strcmp("..", name))))
    {
        return 0;
    }

    int result = uniconf_check(path, name);
    if (result < 0)
    {
        return result; // error or not found
    }
    return result ? uniconf_dir(node, path, name)
                  : uniconf_file(node, path, name);
}

/**
 * Process the directory
 *
 * @param root
 * @param path
 * @param name
 *
 * @return <0 = error, >=0 = count
 */
static int uniconf_dir(uniconf_t root, const char *path, const char *name)
{
    int ret = 0;
    int count = 0;

    char *pathname = uniconf_makepath(path, name);
    if (pathname)
    {
        if (name)
        {
            char *branch = strdup(name);
            char *ext = strchr(branch, '.');
            if (ext)
            {
                ext[0] = '\0';
                root = uniconf_node(root, branch);
            }
            free(branch);
        }

        struct dirent **namelist;

        int n = scandir(pathname, &namelist, NULL, NULL);
        if (n < 0)
        {
            free(pathname);
            return -errno;
        }

        for (int i = 0; i < n; i++)
        {
            if (ret >= 0)
            {
                ret = uniconf_process(root, pathname, namelist[i]->d_name);
                count += ret > 0 ? ret : 0;
            }
            free(namelist[i]);
        }
        free(namelist);
        free(pathname);
    }
    return ret < 0 ? ret : count;
}

/**
 * Parse the file by the extension
 *
 * @param root
 * @param path
 *
 * @param filename
 *
 * @return <0 = error, >=0 = count
 */
static int uniconf_file(uniconf_t root, const char *path, const char *filename)
{
    int ret = 0;

    char *name = strdup(filename);
    char *ext = strrchr(name, '.');
    if (ext)
    {
        ext[0] = '\0';
        ext++;

        char *filepath = uniconf_makepath(path, filename);

        if (!strcmp("env", ext))
        {
            ret = uniconf_env(root, filepath, name);
        }
        else if (!strcmp("ini", ext))
        {
            ret = uniconf_ini(root, filepath, name);
        }
        else if (!strcmp("conf", ext))
        {
            ret = uniconf_conf(root, filepath, name);
        }
        else if (!strcmp("json", ext))
        {
            ret = uniconf_json(root, filepath, name);
        }
        else if (!strcmp("yml", ext) || !strcmp("yaml", ext))
        {
            ret = uniconf_yml(root, filepath, name);
        }

        free(filepath);
    }

    free(name);
    return ret;
}

/**
 * Load config from path
 *
 * @param format
 * @param ...
 *
 * @return : >=0 - success count, <0 - error number
 */
int uniconf_construct(const char *format, ...)
{
    // clear previous
    if (uniconf_root)
    {
        cJSON_Delete(uniconf_root);
    }
    if (uniconf_path)
    {
        free(uniconf_path);
        uniconf_path = NULL;
    }

    // construct
    uniconf_root = cJSON_CreateObject();

    if (format)
    {
        va_list ap;
        va_start(ap, format);
        vasprintf(&uniconf_path, format, ap);
        va_end(ap);

        return uniconf_process(uniconf_root, uniconf_path, NULL);
    }
    return 0;
}

/**
 * Destruct config tree
 *
 */
void uniconf_destruct()
{
    cJSON_Delete(uniconf_root);
    uniconf_root = NULL;
}

static uniconf_t uniconf_object_v(uniconf_t object, const char *format, va_list ap)
{
    char *the_path = NULL;
    vasprintf(&the_path, format, ap);
    for (char *sptr, *token = strtok_r(the_path, PATH_DELIM, &sptr); object && token; token = strtok_r(NULL, PATH_DELIM, &sptr))
    {
        object = cJSON_GetObjectItemCaseSensitive(object, token);
    }
    if (the_path)
    {
        free(the_path);
        the_path = NULL;
    }

    return object;
}

/**
 * Get the named object from config
 *
 * @param format
 * @param ...
 * @return uniconf_t
 */
uniconf_t uniconf_getObject(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    uniconf_t object = uniconf_object_v(uniconf_get_root(), format, ap);
    va_end(ap);

    return object;
}

/**
 * Get the string value of the named object from config
 *
 * @param format
 * @param ...
 * @return char*
 */
char *uniconf_getString(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    uniconf_t object = uniconf_object_v(uniconf_get_root(), format, ap);
    va_end(ap);

    return cJSON_IsString(object) ? cJSON_GetStringValue(object) : NULL;
}

/**
 * Get the number value of the named object from config
 *
 * @param format
 * @param ...
 * @return long long
 */
long long uniconf_getNumber(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    uniconf_t object = uniconf_object_v(uniconf_get_root(), format, ap);
    va_end(ap);

    if (cJSON_IsString(object))
    {
        return atoll(cJSON_GetStringValue(object));
    }
    else if (cJSON_IsNumber(object))
    {
        return (long long)cJSON_GetNumberValue(object);
    }

    return 0;
}

