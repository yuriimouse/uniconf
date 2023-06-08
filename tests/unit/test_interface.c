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

#include "defines.h"

#define HOME_PATH "tests/unit/data/"

static void test_construct(void)
{
    int result = uniconf_construct(NULL);
    CU_ASSERT_EQUAL(0, result);

    cJSON *root = uniconf_get_root();
    char *expect = "{}";
    char *actual = cJSON_PrintUnformatted(root);
    CU_ASSERT_STRING_EQUAL(expect, actual);
    free(actual);

    uniconf_destruct();
}

static void test_env(void)
{
    char *expect = NULL;
    char *path = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %m[^\n]", &path, &expect);
        printf("'%s'->'%s'", path, expect);
        uniconf_construct(path);
        char *actual = cJSON_PrintUnformatted(uniconf_get_root());
        printf("<-'%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    uniconf_destruct();
    FREE_TEST_DATA(path);
    FREE_TEST_DATA(expect);
}

static void test_ini(void)
{
    char *expect = NULL;
    char *path = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %m[^\n]", &path, &expect);
        printf("'%s'->'%s'", path, expect);
        uniconf_construct(path);
        char *actual = cJSON_PrintUnformatted(uniconf_get_root());
        printf("<-'%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    uniconf_destruct();
    FREE_TEST_DATA(path);
    FREE_TEST_DATA(expect);
}

static void test_conf(void)
{
    char *expect = NULL;
    char *path = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %m[^\n]", &path, &expect);
        printf("'%s'->'%s'", path, expect);
        uniconf_construct(path);
        char *actual = cJSON_PrintUnformatted(uniconf_get_root());
        printf("<-'%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    uniconf_destruct();
    FREE_TEST_DATA(path);
    FREE_TEST_DATA(expect);
}

static void test_json(void)
{
    char *expect = NULL;
    char *path = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %m[^\n]", &path, &expect);
        printf("'%s'->'%s'", path, expect);
        uniconf_construct(path);
        char *actual = cJSON_PrintUnformatted(uniconf_get_root());
        printf("<-'%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    uniconf_destruct();
    FREE_TEST_DATA(path);
    FREE_TEST_DATA(expect);
}

static void test_yml(void)
{
    char *expect = NULL;
    char *path = NULL;
    START_USING_TEST_DATA(HOME_PATH)
    {
        USE_OF_THE_TEST_DATA("%ms %m[^\n]", &path, &expect);
        printf("'%s'->'%s'", path, expect);
        uniconf_construct(path);
        char *actual = cJSON_PrintUnformatted(uniconf_get_root());
        printf("<-'%s'\n", actual);
        CU_ASSERT_STRING_EQUAL(expect, actual);
        free(actual);
    }
    FINISH_USING_TEST_DATA;
    uniconf_destruct();
    FREE_TEST_DATA(path);
    FREE_TEST_DATA(expect);
}


CU_TestInfo test_tree[] =
    {
        {"(construct)", test_construct},
        {"(env)", test_env},
        {"(ini)", test_ini},
        {"(conf)", test_conf},
        {"(json)", test_json},
        {"(yml)", test_yml},

        CU_TEST_INFO_NULL,
};
