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

#include <cjson/cJSON.h>
#include <stdlib.h>
#include "uniconf.h"

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

#define FREE_AND_NULL(var) \
    if (var)               \
    {                      \
        free(var);         \
        var = NULL;        \
    }

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

#endif // UNICONF_INTERNAL_H
