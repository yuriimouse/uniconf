#include "uniconf.internal.h"
#include <stdarg.h>
#include <stdio.h>

static void uniconf_error_v(const char *format, va_list ap);
void uniconf_error(const char *format, ...);
void uniconf_error_file(const char *filename, int line, const char *message, ...);

/**
 * Store string to the "error" array
 *
 * @param format
 * @param ap
 */
static void uniconf_error_v(const char *format, va_list ap)
{
    cJSON *root = uniconf_get_root();
    if (root && format && *format)
    {
        cJSON *errors = cJSON_GetObjectItemCaseSensitive(root, "errors");
        if (!errors)
        {
            errors = cJSON_AddArrayToObject(root, "errors");
        }

        char *text = NULL;
        vasprintf(&text, format, ap);

        cJSON_AddItemToArray(errors, cJSON_CreateString(text));

        FREE_AND_NULL(text);
    }
}

/**
 * Add the free error string
 *
 * @param format
 * @param ...
 */
void uniconf_error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    uniconf_error_v(format, ap);
    va_end(ap);
}

/**
 * Add the error found in the file
 *
 * @param filename
 * @param line
 * @param message
 */
void uniconf_error_file(const char *filename, int line, const char *message, ...)
{
    va_list ap;
    va_start(ap, message);
    char *text = NULL;
    vasprintf(&text, message, ap);
    va_end(ap);

    uniconf_error("ERROR: in file '%s' at line %d: %s", filename, line, text);
    FREE_AND_NULL(text);
}
