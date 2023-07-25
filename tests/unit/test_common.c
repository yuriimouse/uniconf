/**
 * @brief Test
 * @author Yurii Prudius
 **/
#define _GNU_SOURCE

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <CUnit/Basic.h>
#include <uniconf.h>

#include "../../sources/uniconf.internal.h"
#include "defines.h"

#define HOME_PATH "tests/unit/data/"

// char *uniconf_makepath(const char *path, const char *name)
static void test_makepath(void)
{
    char *path = NULL;
    char *name = NULL;
    char *expect = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %ms %ms", &path, &name, &expect);
        char *actual = uniconf_makepath(path, name);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        FREE_TEST_DATA(actual);
    }
    FINISH_USING_TEST_DATA;
    FREE_TEST_DATA(expect);
    FREE_TEST_DATA(name);
    FREE_TEST_DATA(path);
    // cJSON *expect = uniconf_construct()
}

// int uniconf_check(const char *path, const char *name)
static void test_check(void)
{
    char *path = NULL;
    char *name = NULL;
    int expect = 0;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %ms %d", &path, &name, &expect);
        // printf("%s + %s = %d ", path, name, expect);
        int actual = uniconf_check(path, name);
        // printf("-> %d\n", actual);
        CU_ASSERT_EQUAL(expect, actual);
    }
    FINISH_USING_TEST_DATA;
    FREE_TEST_DATA(name);
    FREE_TEST_DATA(path);
}

// cJSON *uniconf_node(cJSON *root, const char *name)
static void test_node(void)
{
    char *path = NULL;
    char *name = NULL;
    char *expect = NULL;
    cJSON *root = cJSON_CreateObject();
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %m[^\n]", &name, &expect);
        // printf("%s = '%s' ", name, expect);
        uniconf_node(root, name);
        char *actual = cJSON_PrintUnformatted(root);
        // printf("-> '%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    FREE_TEST_DATA(expect);
    FREE_TEST_DATA(name);
    FREE_TEST_DATA(path);
    cJSON_Delete(root);
}

// int uniconf_is_commented(char *line, const char *prefix)
static void test_is_commented(void)
{
    char *prefix = NULL;
    char *line = NULL;
    int expect = 0;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%d %ms %m[^\n]", &expect, &prefix, &line);
        printf("'%s'->'%s' = %d ", prefix, line, expect);
        int actual = uniconf_is_commented(line, prefix);
        printf("[%d]\n", actual);
        CU_ASSERT_EQUAL(expect, actual);
    }
    FINISH_USING_TEST_DATA;
    FREE_TEST_DATA(line);
    FREE_TEST_DATA(prefix);
}

// char *uniconf_string(char *str, char *trail)
static void test_trim(void)
{
    char *trail = NULL;
    char *str = NULL;
    char *expect = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("'%m[^']' '%m[^']' '%m[^']'", &trail, &str, &expect);
        printf("'%s'->'%s' = '%s'", trail, str, expect);
        char *actual = uniconf_string(str, trail);
        printf("<-'%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
    }
    FINISH_USING_TEST_DATA;
    FREE_TEST_DATA(trail);
    FREE_TEST_DATA(str);
    FREE_TEST_DATA(expect);
}

// char *uniconf_unquote(char *str)
static void test_unquote(void)
{
    char *trail = NULL;
    char *str = NULL;
    char *expect = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("<%m[^>]> <%m[^>]>", &str, &expect);
        printf("[%s]->[%s]", str, expect);
        char *actual = uniconf_unquote(str);
        printf("<-[%s]\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
    }
    FINISH_USING_TEST_DATA;
    FREE_TEST_DATA(trail);
    FREE_TEST_DATA(str);
    FREE_TEST_DATA(expect);
}

// int uniconf_set(cJSON *node, char *name, char *value)
static void test_set(void)
{
    char *name = NULL;
    char *value = NULL;
    char *expect = NULL;
    cJSON *root = cJSON_CreateObject();

    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms '%m[^']' '%m[^']'", &name, &value, &expect);
        printf("[%s]->[%s] = [%s]", name, value, expect);
        uniconf_set(root, name, value);
        char *actual = cJSON_PrintUnformatted(root);
        printf("<-[%s]\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    cJSON_Delete(root);
    FREE_TEST_DATA(name);
    FREE_TEST_DATA(value);
    FREE_TEST_DATA(expect);
}

// cJSON *uniconf_vardata(cJSON *root, char *varname)
static void test_vardata(void)
{
    char *varname = NULL;
    char *expect = NULL;
    cJSON *root = cJSON_CreateObject();
    uniconf_set(root, "foo", "FOOVAL");
    uniconf_set(root, "bar", "BARVAL");
    uniconf_set(root, "baz", "BAZVAL");
    char *actual = cJSON_PrintUnformatted(root);
    printf("===\n%s\n===\n", actual);
    free(actual);

    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms '%m[^']'", &varname, &expect);
        printf("[%s]->[%s]", varname, expect);
        cJSON *json = uniconf_vardata(root, varname);
        actual = cJSON_PrintUnformatted(json);
        printf("<-[%s]\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    cJSON_Delete(root);
    FREE_TEST_DATA(varname);
    FREE_TEST_DATA(expect);
}

// char *uniconf_substitute(cJSON *root, const char *str)
static void test_substitute(void)
{
    char *str = NULL;
    char *expect = NULL;
    cJSON *root = cJSON_CreateObject();
    uniconf_set(root, "foo", "FOOVAL");
    uniconf_set(root, "bar", "BARVAL");
    uniconf_set(root, "baz", "BAZVAL");
    char *actual = cJSON_PrintUnformatted(root);
    printf("===\n%s\n===\n", actual);
    free(actual);

    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("'%m[^']' '%m[^']'", &str, &expect);
        printf("[%s]->[%s]", str, expect);
        actual = uniconf_substitute(root, str);
        printf("<-[%s]\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    cJSON_Delete(root);
    FREE_TEST_DATA(str);
    FREE_TEST_DATA(expect);
}

CU_TestInfo test_common[] =
    {
        {"(makepath)", test_makepath},
        {"(check)", test_check},
        {"(node)", test_node},
        {"(is commented)", test_is_commented},
        {"(trim)", test_trim},
        {"(unquote)", test_unquote},
        {"(set)", test_set},
        {"(vardata)", test_vardata},
        {"(substitute)", test_substitute},

        CU_TEST_INFO_NULL,
};
