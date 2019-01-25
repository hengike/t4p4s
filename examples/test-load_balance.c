// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Eotvos Lorand University, Budapest, Hungary

#include "test.h"

fake_cmd_t t4p4s_testcase_test[][RTE_MAX_LCORE] = {
    {

        {FAKE_PKT, 0, 0, ETH(ETH1A, ETH01), 200, 0, ETH(ETHNODE, ETH01)},
        FEND,
    },

    {
        {FAKE_PKT, 0, 0, ETH(ETH1A, ETH03), 0, 0, ETH(ETHNODE, ETH03)},
        FEND,
    },
};


testcase_t t4p4s_test_suite[MAX_TESTCASES] = {
    { "test",           &t4p4s_testcase_test },
    TEST_SUITE_END,
};
