//
// Created by volund on 10/14/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "rhope.h"
#include <json-c/json.h>



void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    fwrite(buf->base, 1, nread, stdout);
    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(server->loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

void start_server() {
    uv_loop_t *loop = uv_default_loop();
    uv_tcp_t server;
    uv_tcp_init(loop, &server);
    int err;

    struct sockaddr_in bind_addr;
    err = uv_ip4_addr("10.0.0.226", 4100, &bind_addr);
    if(err) {
        fprintf(stderr, "Error binding %s\n", uv_strerror(err));
        return;
    }

    uv_tcp_bind(&server, (const struct sockaddr*)&bind_addr, 0);
    err = uv_listen((uv_stream_t*)&server, 10, on_new_connection);
    if(err) {
        fprintf(stderr, "listen error %s\n", uv_strerror(err));
        return;
    }
    uv_run(loop, UV_RUN_DEFAULT);

}



int main() {
    rhope_state_t state = {0};
    state.loop = uv_default_loop();
    rh_dict_insert(&state.protocols, "test", (void*)&state);
    printf("Now quitting.\n");
    return 0;
}