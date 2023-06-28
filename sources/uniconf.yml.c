#include "uniconf.internal.h"

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <lists.h>

struct yaml_level
{
    int prefix;
    cJSON *node;
};

static cJSON *uniconf_yml__string(char *value)
{
    if (value && *value)
    {
        char *sdup = strdup(value);
        char *trimed = uniconf_trim(sdup, "\r\n");
        cJSON *item = NULL;
        int slen = strlen(trimed);
        if ((slen > 2) && ('"' == trimed[0]) && ('"' == trimed[slen - 1]))
        {
            char *expanded = uniconf_substitute(NULL, trimed);
            item = cJSON_CreateString(uniconf_unquote(expanded));
            FREE_AND_NULL(expanded);
        }
        else
        {
            item = cJSON_CreateString(uniconf_unquote(trimed));
        }
        FREE_AND_NULL(sdup);
        return item;
    }
    return cJSON_CreateNull();
}

static cJSON *uniconf_yml__object(cJSON *node, char *name, char *value)
{
    cJSON *item = uniconf_yml__string(value);
    if (cJSON_AddItemToObject(node, name, item))
    {
        return item;
    }
    cJSON_Delete(item);
    return NULL;
}

static cJSON *uniconf_yml__array(cJSON *node, char *value)
{
    cJSON *item = uniconf_yml__string(value);
    if (cJSON_AddItemToArray(node, item))
    {
        return item;
    }
    cJSON_Delete(item);
    return NULL;
}

static cJSON *uniconf_yml__add(cJSON *node, char *name, char *value)
{
    if (STR_EQUAL(name, "-"))
    {
        // array element
        if (cJSON_IsNull(node))
        {
            // starts as array
            node->type = cJSON_Array;
        }
        return cJSON_IsArray(node) ? uniconf_yml__array(node, value) : NULL;
    }
    else // named
    {
        char *pure = uniconf_trim(name, ":");
        // object element
        if (cJSON_IsNull(node))
        {
            // starts as object
            node->type = cJSON_Object;
        }
        return cJSON_IsObject(node) ? uniconf_yml__object(node, pure, value) : NULL;
    }
    return NULL;
}

static list_t *stack = NULL;
static struct yaml_level *stack_push(int pfx, cJSON *node)
{
    if (!stack)
    {
        stack = list_construct();
    }
    struct yaml_level *level = malloc(sizeof(struct yaml_level));
    level->prefix = pfx;
    level->node = node;
    list_push(stack, (void *)level);

    return level;
}
static struct yaml_level *stack_get()
{
    if (!stack)
    {
        stack = list_construct();
    }
    return (struct yaml_level *)list_get(stack);
}
// static struct yaml_level *stack_pop()
// {
//     if (!stack)
//     {
//         stack = list_construct();
//     }
//     return (struct yaml_level *)list_pop(stack);
// }
static struct yaml_level *stack_pop_get()
{
    void *lvl = list_pop(stack);
    FREE_AND_NULL(lvl);
    return (struct yaml_level *)list_get(stack);
}

/**
 * Parse the .yml file
 *
 * @param root
 * @param filepath
 * @param branch
 * @return int
 */
int uniconf_yml(cJSON *root, const char *filepath, const char *branch)
{
    int count = 0;
    cJSON *node = uniconf_node(root, branch);
    if (node)
    {
        int pfxlen = 0;
        char *name = NULL;
        char *value = NULL;
        cJSON *last = NULL;
        struct yaml_level *level = NULL;
        uniconf_FileByLine(filepath, line)
        {
            if (!(uniconf_is_commented(line, "#") || uniconf_is_commented(line, "---")))
            {
                pfxlen = 0;
                FREE_AND_NULL(name);
                FREE_AND_NULL(value);
                sscanf(line, "%*[ ]%n", &pfxlen);
                sscanf(line + pfxlen, "%ms %m[^#\r\n]", &name, &value);

                if (name)
                {
                    level = stack_get(); // current level
                    if (!level)
                    {
                        // first line
                        if (pfxlen > 0)
                        {
                            uniconf_error_file(filepath, _lineno, "leading spaces are not allowed in the first line");
                            break;
                        }
                        level = stack_push(0, node);
                        last = uniconf_yml__add(node, name, value);
                    }
                    else if (pfxlen == level->prefix)
                    {
                        // same level
                        last = uniconf_yml__add(level->node, name, value);
                        if (!last)
                        {
                            uniconf_error_file(filepath, _lineno, "can't add it to the current level ('%s':'%s')", name, value);
                            break;
                        }
                    }
                    else if (pfxlen > level->prefix)
                    {
                        // next level
                        level = stack_push(pfxlen, last);
                        last = uniconf_yml__add(level->node, name, value);
                        if (!last)
                        {
                            uniconf_error_file(filepath, _lineno, "can't expand the current level");
                            break;
                        }
                    }
                    else // if(pfxlen < level->prefix)
                    {
                        // previous level
                        while ((pfxlen < level->prefix))
                        {
                            level = stack_pop_get();
                        }
                        if (pfxlen != level->prefix)
                        {
                            uniconf_error_file(filepath, _lineno, "the uncorrect level");
                            break;
                        }
                        last = uniconf_yml__add(level->node, name, value);
                        if (!last)
                        {
                            uniconf_error_file(filepath, _lineno, "can't continue expanding the current level");
                            break;
                        }
                    }
                }
            }
        }
        uniconf_EndByLine(line);
        FREE_AND_NULL(name);
        FREE_AND_NULL(value);
        stack = list_destruct(stack, free);
    }

    return count;
}
