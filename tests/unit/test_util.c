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

static void test_Trees(void)
{
    cJSON *expect = uniconf_construct()
    CU_ASSERT_STRING_EQUAL(expect, actual);
}

CU_TestInfo test_tree[] =
    {
        {"(tree)", test_Trees},

        CU_TEST_INFO_NULL,
};
