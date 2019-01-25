// Copyright 2016 Eotvos Lorand University, Budapest, Hungary
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
#include "controller.h"
#include "messages.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_IPS 60000

controller c;


void fill_ecmp_group_table(uint8_t new_addr[6], uint8_t route1, uint8_t route2, uint8_t node_id)
{
    printf("FILL");
    char buffer[2048]; /* TODO: ugly */
    struct p4_header* h;
    struct p4_add_table_entry* te;
    struct p4_action* a;
    struct p4_action_parameter* ap0;
    struct p4_action_parameter* ap1;
    struct p4_action_parameter* ap2;
    struct p4_field_match_exact* exact;

    printf("new_addr: \n");
    for(int j = 0; j < 6; j++) {
        printf("%hhx ", new_addr[j]);
    }
    printf("\n");
    printf("filling Route1: %hhd  Route2: %hhd node_id %hhd\n", route1 , route2, node_id);

    printf("ecmp_group\n");
    h = create_p4_header(buffer, 0, 2048);
    te = create_p4_add_table_entry(buffer,0,2048);
    strcpy(te->table_name, "ecmp_group");
;
    exact = add_p4_field_match_exact(te, 2048);
    strcpy(exact->header.name, "routing_metadata.node_id");
    memcpy(exact->bitmap, &node_id, 1);
    exact->length = 1*8+0;

    a = add_p4_action(h, 2048);
    strcpy(a->description.name, "forward");

    ap0 = add_p4_action_parameter(h, a, 2048);
    strcpy(ap0->name, "new_addr");
    memcpy(ap0->bitmap, &new_addr, 6);
    ap0->length = 6*8+0;

    ap1 = add_p4_action_parameter(h, a, 2048);   
    strcpy(ap1->name, "route1");
    memcpy(ap1->bitmap, &route1, 2);
    ap1->length = 2*8+0;

    ap2 = add_p4_action_parameter(h, a, 2048);   
    strcpy(ap2->name, "route2");
    memcpy(ap2->bitmap, &route2, 2);
    ap2->length = 2*8+0;


    netconv_p4_header(h);
    printf("h\n");
    netconv_p4_add_table_entry(te);
    printf("te\n");
    netconv_p4_field_match_exact(exact);
    netconv_p4_action(a);
    printf("1\n");
    netconv_p4_action_parameter(ap0);
    printf("ap0\n");
    netconv_p4_action_parameter(ap1);
    printf("ap1\n");
    netconv_p4_action_parameter(ap2);
    printf("ap2\n");

    send_p4_msg(c, buffer, 2048);
    sleep(1200);
    printf("Buffer content: %s" , buffer);
    printf("send\n");
    //sleep(1200);
}


void set_default_action_ecmp_group()
{
        char buffer[2048];
        struct p4_header* h;
        struct p4_set_default_action* sda;
        struct p4_action* a;

        printf("Generate set_default_action message for table ecmp_group\n");

        h = create_p4_header(buffer, 0, sizeof(buffer));

        sda = create_p4_set_default_action(buffer,0,sizeof(buffer));
        strcpy(sda->table_name, "ecmp_group");

        a = &(sda->action);
        strcpy(a->description.name, "nop");

        netconv_p4_header(h);
        netconv_p4_set_default_action(sda);
        netconv_p4_action(a);

        send_p4_msg(c, buffer, sizeof(buffer));
}



void dhf(void* b) {
       printf("Unknown digest received\n");
}

void init_simple() {
    uint8_t new_addr[6] = {0xd2, 0x69, 0x0f, 0xa8, 0x39, 0x9c};
    uint8_t route1 = 15;
    uint8_t route2 = 16;
    uint8_t node_id = 9;

    fill_ecmp_group_table(new_addr, route1, route2, node_id);
}


int read_config_from_file(char *filename) {
    FILE *f;
    char line[100];
        uint8_t new_addr[6];
        uint8_t route1;
        uint8_t route2;
        uint8_t node_id;
        char dummy;

        printf("READING\n");
        f = fopen(filename, "r");
        if (f == NULL) return -1;

        int line_index = 0;
        while (fgets(line, sizeof(line), f)) {
                line[strlen(line)-1] = '\0';
                line_index++;
                printf("Sor: %hhd.\n",line_index);
                if (10 == sscanf(line, "%c %hhx:%hhx:%hhx:%hhx:%hhx:%hhx %hhd %hhd %hhd",
                                &dummy, &new_addr[0], &new_addr[1], &new_addr[2], &new_addr[3], &new_addr[4], &new_addr[5], &route1 , &route2, &node_id) )
                {
                    printf("new_addr: \n");
                    for(int j = 0; j < 6; j++) {
                        printf("%hhx ", new_addr[j]);
                    }
                    printf("\n");
                    printf("filling Route1: %hhd  Route2: %hhd node_id: %hhd\n", route1 , route2, node_id);

                    fill_ecmp_group_table(new_addr, route1, route2, node_id);
                    printf("postfil\n");
                }
                else {
                    printf("Wrong format error in line\n");
                    fclose(f);
                    return -1;
                }
        }
    fclose(f);
    return 0;
}

char* fn;

void init_complex() {
    set_default_action_ecmp_group();
    if (read_config_from_file(fn)<0) {
         printf("File cannnot be opened...\n");
    }
}


int main(int argc, char* argv[]) 
{
    printf("Create and configure controller...\n");

    if (argc>1) {
        if (argc!=2) {
            printf("Too many arguments...\nUsage: %s <filename(optional)>\n", argv[0]);
            return -1;
        }
        printf("Command line argument is present...\nLoading configuration data...\n");
                fn = argv[1];
        c = create_controller_with_init(11111, 3, dhf, init_complex);
    }
    else {
        c = create_controller_with_init(11111, 3, dhf, init_simple);
    }

    printf("Launching controller's main loop...\n");
    execute_controller(c);

    printf("Destroy controller\n");
    destroy_controller(c);

    return 0;
}
