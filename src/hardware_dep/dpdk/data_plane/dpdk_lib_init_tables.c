// Copyright 2018 Eotvos Lorand University, Budapest, Hungary
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// This file is included directly from `dpdk_lib.c`.


extern void create_table(lookup_table_t* t, int socketid);

void create_tables_on_socket(int socketid)
{
    if(table_config == NULL) return;

    debug(" :::: Initializing tables on socket " T4LIT(%d,socket) "...\n", socketid);
    for (int i = 0; i < NB_TABLES; i++) {
        lookup_table_t t = table_config[i];

        debug("   :: Creating instances for table " T4LIT(%s,table) " on socket " T4LIT(%d,socket) " (" T4LIT(%d) " copies)\n", t.name, socketid, NB_REPLICA);
        for (int j = 0; j < NB_REPLICA; j++) {
            state[socketid].tables[i][j] = malloc(sizeof(lookup_table_t));
            memcpy(state[socketid].tables[i][j], &t, sizeof(lookup_table_t));
            state[socketid].tables[i][j]->instance = j;
            create_table(state[socketid].tables[i][j], socketid);
        }
        state[socketid].active_replica[i] = 0;
    }
}

void create_table_on_lcore(unsigned lcore_id)
{
    if (rte_lcore_is_enabled(lcore_id) == 0) return;

    int socketid = get_socketid(lcore_id);

    if (state[socketid].tables[0][0] == NULL) {
        create_tables_on_socket(socketid);
    }
}

void init_tables()
{
    debug(" :::: Initializing stateful memories...\n");
    for (unsigned lcore_id = 0; lcore_id < RTE_MAX_LCORE; lcore_id++) {
        create_table_on_lcore(lcore_id);
    }
}
