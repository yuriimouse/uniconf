/**
 * @brief 
 * @author Yurii Prudius
 **/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <helper.h>
#include <CUnit/Basic.h>

#include "defines.h"
#include <rules.h>

static void test_contains(void)
{
    cJSON *data = load_varData("tests/unit/data/var_data.json");

    char *etalon = NULL;
    char *varname = NULL;
    int expect = 0;

    START_USING_TEST_DATA("tests/unit/data/")
    {
        USE_OF_THE_TEST_DATA("%m[^=]= %m[^=]= %d", &etalon, &varname, &expect);

        cJSON *var = rule_getVar(data, varname);
        int actual = EXTERN_cond_contains(etalon, var, 0);

        string json = var ? cJSON_PrintUnformatted(var) : NULL;
        printf("'%s'~'%s'(%d=%d)\n", etalon, json, expect, actual);
        json = stringFree(json);
        CU_ASSERT_EQUAL(expect, actual);

        if (etalon)
        {
            free(etalon);
            etalon = NULL;
        }
        if (varname)
        {
            free(varname);
            varname = NULL;
        }
    }
    FINISH_USING_TEST_DATA;

    if (data)
    {
        cJSON_Delete(data);
        data = NULL;
    }
}

static void test_contains_count(void)
{
    cJSON *data = load_varData("tests/unit/data/var_data.json");

    char *etalon = NULL;
    char *varname = NULL;
    int expect = 0;

    START_USING_TEST_DATA("tests/unit/data/")
    {
        USE_OF_THE_TEST_DATA("%m[^=]= %m[^=]= %d", &etalon, &varname, &expect);

        cJSON *var = rule_getVar(data, varname);
        int actual = EXTERN_cond_contains(etalon, var, 1);

        string json = var ? cJSON_PrintUnformatted(var) : NULL;
        printf("'%s'~'%s'(%d=%d)\n", etalon, json, expect, actual);
        json = stringFree(json);
        CU_ASSERT_EQUAL(expect, actual);

        if (etalon)
        {
            free(etalon);
            etalon = NULL;
        }
        if (varname)
        {
            free(varname);
            varname = NULL;
        }
    }
    FINISH_USING_TEST_DATA;

    if (data)
    {
        cJSON_Delete(data);
        data = NULL;
    }
}

static void test_math(void)
{
    cJSON *data = load_varData("tests/unit/data/var_data.json");

    char *math = NULL;
    char act = '\0';
    char *right = NULL;
    int expect = 0;

    START_USING_TEST_DATA("tests/unit/data/")
    {
        USE_OF_THE_TEST_DATA("%m[^|]| %c %m[^=]=%d", &math, &act, &right, &expect);

        int actual = EXTERN_cond_math(math, act, right, data);

        printf("'%s'%c'%s'(%d=%d)\n", math, act, right, expect, actual);
        CU_ASSERT_EQUAL(expect, actual);

        math = stringFree(math);
        right = stringFree(right);
    }
    FINISH_USING_TEST_DATA;

    if (data)
    {
        cJSON_Delete(data);
        data = NULL;
    }
}

CU_TestInfo test_find[] =
    {
        {"(cond_contains)", test_contains},
        {"(cond_contains count)", test_contains_count},
        {"(cond_math)", test_math},

        CU_TEST_INFO_NULL,
};
