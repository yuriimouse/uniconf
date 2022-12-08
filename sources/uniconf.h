#ifndef UNICONF_H
#define UNICONF_H
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

#define _GNU_SOURCE

#include <stdlib.h>
#include <cjson/cJSON.h>

typedef cJSON *
    uniconf_t;

uniconf_t uniconf_get_root();

// internal
int uniconf_process(uniconf_t root, const char *path, const char *name);
int uniconf_file(uniconf_t root, const char *path, const char *filename);
int uniconf_dir(uniconf_t root, const char *path, const char *name);

// common utils
char *uniconf_makepath(const char *path, const char *name);
int uniconf_check(const char *path, const char *name);
cJSON *uniconf_node(cJSON *root, const char *name);
int uniconf_is_commented(char *line, const char *prefix);
char *uniconf_trim(char *str, char *trail);
char *uniconf_unquote(char *str);
char *uniconf_substitute(const char *str);
char *uniconf_get_vardata(char *str, int len, char *not_found, ...);
cJSON *uniconf_vardata(char *varname);
int uniconf_set(cJSON *node, char *name, char *value);

// parsers
int uniconf_env(cJSON *root, const char *filepath, const char *branch);
int uniconf_ini(cJSON *root, const char *filepath, const char *branch);
int uniconf_json(cJSON *root, const char *filepath, const char *branch);
int uniconf_conf(cJSON *root, const char *filepath, const char *branch);
int uniconf_yml(cJSON *root, const char *filepath, const char *branch);

// interface
int uniconf_construct(const char *format, ...);
void uniconf_destruct();

uniconf_t uniconf_object(const char *format, ...);
char *uniconf_value(const char *format, ...);

#define uniconf_IsObject(element) ((element)->type & (cJSON_Array | cJSON_Object))
#define uniconf_GetName(element) ((element)->string)
#define uniconf_GetValue(element) ((element)->valuestring)
#define uniconf_GetNumber(element) ((element)->type & cJSON_Number ? (long long)(element)->valuedouble : atoll((element)->valuestring))
#define uniconf_GetObject(element) ((element)->child)
#define uniconf_ForEach(element, array) for (cJSON *element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

#define uniconf_FileByLine(filepath, linevar)        \
    FILE *_file = NULL;                              \
    if (filepath && (_file = fopen(filepath, "rt"))) \
    {                                                \
        char *linevar = NULL;                        \
        size_t _len = 0;                             \
        while (-1 != getline(&linevar, &_len, _file))
#define uniconf_EndByLine(linevar) \
    free(linevar);                 \
    fclose(_file);                 \
    }

#endif // UNICONF_H