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

#include <cjson/cJSON.h>
#include <stdlib.h>

#define PATH_DELIM "./:\\ "

typedef cJSON *uniconf_t;

uniconf_t uniconf_get_root();

// interface
int uniconf_construct(const char *format, ...);
void uniconf_destruct();

uniconf_t uniconf_getObject(const char *format, ...);
char *uniconf_getString(const char *format, ...);
long long uniconf_getNumber(const char *format, ...);
int uniconf_getBoolean(const char *format, ...);

#define uniconf_IsArray(element) cJSON_IsArray(element)
#define uniconf_IsObject(element) cJSON_IsObject(element)
#define uniconf_IsComplex(element) (cJSON_IsArray(element) || cJSON_IsObject(element))
#define uniconf_GetName(element) ((element)->string)
#define uniconf_ForEach(element, array) for (cJSON *element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

#endif // UNICONF_H
