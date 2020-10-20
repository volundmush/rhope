//
// Created by volund on 10/18/20.
//

#ifndef RHOPE_RHOPE_H
#define RHOPE_RHOPE_H

#include <uv.h>
#include <string.h>
#include <stdlib.h>



typedef struct rh_kvnode_s {
    struct rh_kvnode_s *next;
    char *key;
    void *value;
} rh_kvnode_t;

typedef struct rh_dict_s {
    rh_kvnode_t *head;
    rh_kvnode_t *tail;
} rh_dict_t;

rh_kvnode_t *rh_dict_get_node(rh_dict_t *in_holder, char *in_key);

void *rh_dict_get_value(rh_dict_t *in_dict, char *in_key);

int rh_dict_get_or_create(rh_dict_t *in_dict, char *in_key, rh_kvnode_t **in_out);

// Inserts - or replaces an existing - key with the given value into *holder.
int rh_dict_insert(rh_dict_t *in_dict, char *in_key, void *in_value);

// this struct is used to hold the global state of the Rhope server.

typedef struct rh_state_s {
    uv_loop_t *loop;
    rh_dict_t plugins;
    rh_dict_t interfaces;
    rh_dict_t protocols;
    rh_dict_t tls;
    rh_dict_t servers;
} rh_state_t;

typedef struct rh_interface_s {
    char *name;
    char *address;
} rh_interface_t;

typedef struct rh_protocol_s {
    char *name;
    uv_connect_cb on_connect;
    uv_connection_cb on_accept;
    uv_read_cb on_read;
    uv_write_cb on_write;
    uv_shutdown_cb on_shutdown;
} rh_protocol_t;

typedef struct rh_server_s {
    char *name;
    rh_protocol_t *protocol;
    rh_interface_t *interface;
    unsigned short port;
    uv_tcp_t *handle;
} rh_server_t;

typedef void (*rh_plugin_init_cb)(rh_state_t*);
typedef void (*rh_plugin_start_cb)(rh_state_t*);
typedef void (*rh_plugin_shutdown_cb)(rh_state_t*);

typedef struct rh_plugin_s {
    char *name;
    char *filename;
    uv_lib_t *lib;
    rh_plugin_init_cb init_cb;
    rh_plugin_start_cb start_cb;
    rh_plugin_shutdown_cb shutdown_cb;
} rh_plugin_t;

#endif //RHOPE_RHOPE_H
