//
// Created by volund on 10/14/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "rhope.h"
#include "config.h"
#include <unistd.h>
#include <sys/stat.h>

int main() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("getcwd() error");
    else
        printf("current working directory is: %s\n", cwd);

    struct stat st;
    stat("config.json", &st);

    char *js = calloc(1, st.st_size+1);
    FILE *f = fopen("config.json", "rb");
    fread(js, 1, st.st_size, f);

    rh_state_t state = {0};
    state.loop = uv_default_loop();
    int err = 0;
    if((err = rh_load_json(&state, js))) {
        return -1;
    }
    rh_protocol_t *test = (rh_protocol_t*)rh_dict_get_value(&state.protocols, "mudtelnet");
    if((err = rh_setup_servers(&state))) {
        printf("problem setting up servers");
        return -1;
    }
    uv_run(state.loop, UV_RUN_DEFAULT);
    printf("Now quitting.\n");
    return 0;
}