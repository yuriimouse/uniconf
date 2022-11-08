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

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

static char *uniconf_path = NULL;
static uniconf_t
    uniconf_root = NULL;

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
 * Process the directory
 *
 * @param root
 * @param path
 * @param name
 *
 * @return <0 = error, >=0 = count
 */
int uniconf_dir(uniconf_t root, const char *path, const char *name)
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
                root = cJSON_AddObjectToObject(root, branch);
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
int uniconf_file(uniconf_t root, const char *path, const char *filename)
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
            ret = parse_env(root, filepath, name);
        }
        else if (!strcmp("ini", ext))
        {
            ret = parse_ini(root, filepath, name);
        }
        else if (!strcmp("conf", ext))
        {
            ret = parse_conf(root, filepath, name);
        }
        else if (!strcmp("json", ext))
        {
            ret = parse_json(root, filepath, name);
        }
        else if (!strcmp("yml", ext) || !strcmp("yaml", ext))
        {
            ret = parse_yml(root, filepath, name);
        }

        free(filepath);
    }

    free(name);
    return ret;
}

/**
 * Process the directory entry into the config node
 *
 * @param node
 * @param path
 * @param name
 *
 * @return <0 - error, >= 0 - count
 */
// uniconf_process(uniconf_root, uniconf_path, NULL)
int uniconf_process(uniconf_t node, const char *path, const char *name)
{
    if (0 == (strcmp(".", name) * strcmp("..", name)))
    {
        // skip
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
 * Load config from path
 *
 * @param format
 * @param ...
 *
 * @return : 0 - success, <>0 - error number
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

    va_list ap;
    va_start(ap, format);
    vasprintf(&uniconf_path, format, ap);
    va_end(ap);

    return uniconf_process(uniconf_root, uniconf_path, NULL);
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

/**
 * Get the named object from config
 *
 * @param format
 * @param ...
 * @return uniconf_t
 */
uniconf_t uniconf_object(const char *format, ...)
{
    char *the_path = NULL;

    va_list ap;
    va_start(ap, format);
    vasprintf(&the_path, format, ap);
    va_end(ap);

    //

    return NULL;
}

/**
 * Get the string value of the named object from config
 *
 * @param format
 * @param ...
 * @return const char*
 */
char *uniconf_value(const char *format, ...)
{
    char *the_path = NULL;

    va_list ap;
    va_start(ap, format);
    vasprintf(&the_path, format, ap);
    va_end(ap);

    //

    return NULL;
}
