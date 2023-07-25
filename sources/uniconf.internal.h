#ifndef UNICONF_INTERNAL_H
#define UNICONF_INTERNAL_H
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

#include "uniconf.h"
#include <cjson/cJSON.h>
#include <stdlib.h>

// common utils
char *uniconf_makepath(const char *path, const char *name);
int uniconf_check(const char *path, const char *name);
int uniconf_is_commented(char *line, const char *prefix);
char *uniconf_string(char *str, char *trail);
char *uniconf_unquote(char *str);
cJSON *uniconf_node(cJSON *root, const char *name);
cJSON *uniconf_nodeNULL(cJSON *root, const char *name);
char *uniconf_substitute(cJSON *root, const char *str);
cJSON *uniconf_vardata(cJSON *root, char *varname);
int uniconf_set(cJSON *node, char *name, char *value);

// errors
void uniconf_error(const char *format, ...);
void uniconf_error_file(const char *filename, int line, const char *message, ...);

// parsers
int uniconf_env(cJSON *root, const char *filepath, const char *branch);
int uniconf_ini(cJSON *root, const char *filepath, const char *branch);
int uniconf_list(cJSON *root, const char *filepath, const char *branch);
int uniconf_json(cJSON *root, const char *filepath, const char *branch);
int uniconf_conf(cJSON *root, const char *filepath, const char *branch);
int uniconf_yml(cJSON *root, const char *filepath, const char *branch);

#define FREE_AND_NULL(var) \
    if (var)               \
    {                      \
        free(var);         \
        var = NULL;        \
    }

#define STR_EQUAL(a,b) (!strcmp(a,b))

#define uniconf_FileByLine(filepath, linevar)        \
    FILE *_file = NULL;                              \
    if (filepath && (_file = fopen(filepath, "rt"))) \
    {                                                \
        char *linevar = NULL;                        \
        size_t _len = 0;                             \
        for (int _lineno = 1; -1 != getline(&linevar, &_len, _file); _lineno++)

#define uniconf_EndByLine(linevar) \
    free(linevar);                 \
    fclose(_file);                 \
    }

#endif // UNICONF_INTERNAL_H
