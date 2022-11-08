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

// statics
int uniconf_check(const char *path, const char *name);
int uniconf_file(uniconf_t root, const char *path, const char *filename);
int uniconf_process(uniconf_t root, const char *path, const char *name);
int uniconf_dir(uniconf_t root, const char *path, const char *name);

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

#endif // UNICONF_H