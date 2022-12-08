/**
 * @brief Test
 * @author Yurii Prudius
 **/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

#include <CUnit/Basic.h>

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
        USE_OF_THE_TEST_DATA("%ms %ms %ms",&path, &name, &expect);
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

CU_TestInfo test_common[] =
    {
        {"(makepath)", test_makepath},

        CU_TEST_INFO_NULL,
};
