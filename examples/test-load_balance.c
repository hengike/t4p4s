// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Eotvos Lorand University, Budapest, Hungary

#include "test.h"

fake_cmd_t fake_commands[][RTE_MAX_LCORE] = {
    {
        FSLEEP(200),
        {FAKE_PKT, 0, 0, ETH(ETH1A, ETH01), 200, 18, ETH(ETH01, ETH1A)},
        {FAKE_PKT, 0, 0, ETH(ETH1A, ETH02), 200, 18, ETH(ETH01, ETH1A)},
        {FAKE_PKT, 0, 0, ETH(ETH01, ETH1A),   0, 18, ETH(ETH01, ETH1A)},
        {FAKE_PKT, 0, 0, ETH(ETH02, ETH1A),   0, 18, ETH(ETH01, ETH1A)},

        FEND,
    },

    {
        {FAKE_PKT, 0, 0, ETH(ETH1A, ETH03), 200, 18, ETH(ETH01, ETH1A)},
        {FAKE_PKT, 0, 0, ETH(ETH1A, ETH04), 200, 18, ETH(ETH01, ETH1A)},
        {FAKE_PKT, 0, 0, ETH(ETH03, ETH1A),   0, 18, ETH(ETH01, ETH1A)},
        {FAKE_PKT, 0, 0, ETH(ETH04, ETH1A),   0, 18, ETH(ETH01, ETH1A)},

        FEND,
    },
};
