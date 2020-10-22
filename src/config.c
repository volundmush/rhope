//
// Created by volund on 10/18/20.
//

#include "config.h"

int rh_load_plugins(rh_state_t *state, json_object *jobj) {
    int err = 0;
    if(!json_object_is_type(jobj, json_type_array)) {
        printf("plugins must be an array of strings");
        return -1;
    }

    for(int i = 0; i < json_object_array_length(jobj); i++) {
        json_object *entry = json_object_array_get_idx(jobj, i);
        if(!json_object_is_type(entry, json_type_string)) {
            printf("plugins must be an array of strings");
            return -1;
        }

        rh_plugin_t *plugin = 0;
        if(!(plugin = calloc(1, sizeof(rh_plugin_t)))) {
            printf("could not allocate memory for plugin");
            return -1;
        }

        if(!(plugin->lib = calloc(1, sizeof(uv_lib_t)))) {
            printf("could not allocate memory for library");
            return -1;
        }

        const int entry_len = json_object_get_string_len(entry);
        if(rh_strcpy(&plugin->name, json_object_get_string(entry))) {
            printf("could not allocate memory for plugin filename");
            return -1;
        }

        if(!(plugin->filename = calloc(entry_len+ 1 + 9, 1))) {
            printf("could not allocate memory for plugin filename");
            return -1;
        }
        strcat(plugin->filename, "./lib");
        strcat(plugin->filename, plugin->name);
        strcat(plugin->filename, ".so");

        if((err = uv_dlopen(plugin->filename, plugin->lib))) {
            printf("error opening library");
            return -1;
        }

        if((err = uv_dlsym(plugin->lib, "plugin_init", (void*)&plugin->init_cb))) {
            printf("could not link plugin_init");
            return -1;
        }

        if((err = uv_dlsym(plugin->lib, "plugin_start", (void*)&plugin->start_cb))) {
            printf("could not link plugin_start");
            return -1;
        }

        if((err = uv_dlsym(plugin->lib, "plugin_shutdown", (void*)&plugin->start_cb))) {
            printf("could not link plugin_shutdown");
            return -1;
        }

        if((err = rh_dict_insert(&state->plugins, plugin->name, (void*)plugin))) {
            printf("could not insert plugin into dictionary");
            return -1;
        }

    }

    rh_dict_foreach(state->plugins.head, k, v, rh_plugin_t) {
        if((err = v->init_cb(state))) {
            printf("Something done goofed");
            return -1;
        }
    }

    return 0;
}

int rh_load_interfaces(rh_state_t *state, json_object *jobj) {
    int err = 0;
    json_object_object_foreach(jobj, key, val) {
        if(!json_object_is_type(val, json_type_string)) {
            printf("interfaces must be an object of strings");
            return -1;
        }

        rh_interface_t *interface = 0;
        if(!(interface = calloc(1, sizeof(rh_interface_t)))) {
            printf("cannot allocate memory for interface");
            return -1;
        }

        if(rh_strcpy(&interface->name, key)) {
            printf("cannot allocate memory for interface name");
            return -1;
        }

        if(rh_strcpy(&interface->address, json_object_get_string(val))) {
            printf("cannot allocate memory for interface address");
            return -1;
        }

        if((err = rh_dict_insert(&state->interfaces, interface->name, (void*)interface))) {
            printf("could not insert plugin into dictionary");
            return -1;
        }
    }

    return 0;
}

int rh_load_tls(rh_state_t *state, json_object *jobj) {
    json_object_object_foreach(jobj, key, val) {
        // currently does nothing.
    }

    return 0;
}

int rh_load_servers(rh_state_t *state, json_object *jobj) {
    struct sockaddr_in *addr = 0;
    rh_server_t *server = 0;
    int err = 0;

    json_object_object_foreach(jobj, key, val) {
        if(!json_object_is_type(val, json_type_object)) {
            printf("server entries must be json objects");
            return -1;
        }

        json_object *cur = 0;
        if(!(server = calloc(1, sizeof(rh_server_t)))) {
            printf("cannot allocate memory for server");
            return -1;
        }

        if(rh_strcpy(&server->name, key)) {
            printf("cannot allocate memory for server name");
            return -1;
        }

        if(!(json_object_object_get_ex(val, "interface", &cur) && json_object_is_type(cur, json_type_string))) {
            printf("server interface must be a string");
            return -1;
        }
        if(!(server->interface = rh_dict_get_value(&state->interfaces, json_object_get_string(cur)))) {
            printf("interface does not exist");
            return -1;
        }

        if(!(json_object_object_get_ex(val, "protocol", &cur) && json_object_is_type(cur, json_type_string))) {
            printf("server protocol must be a string");
            return -1;
        }
        if(!(server->protocol = rh_dict_get_value(&state->protocols, json_object_get_string(cur)))) {
            printf("protocol does not exist");
            return -1;
        }

        if(!(json_object_object_get_ex(val, "port", &cur) && json_object_is_type(cur, json_type_int))) {
            printf("server port must be an integer");
            return -1;
        }

        server->port = json_object_get_int(cur);

        if(!(server->sockaddr = calloc(1, sizeof(struct sockaddr_in)))) {
            printf("failed to allocate memory for sockaddr");
            return -1;
        }

        if((err = uv_ip4_addr(server->interface->address, server->port, server->sockaddr))) {
            fprintf(stderr, "Error binding %s\n", uv_strerror(err));
            return -1;
        }

        if(!(server->handle = calloc(1, sizeof(uv_tcp_t)))) {
            printf("failed to allocate memory for server handle");
            return -1;
        }

        if((err = uv_tcp_init(state->loop, server->handle))) {
            printf("could not initialize tcp server to loop");
            return -1;
        }

        // link the handle to this server definition so it has access to... itself.
        // Yes. pure genius.
        server->handle->data = (void*)server;

        if(rh_dict_insert(&state->servers, server->name, (void*)server)) {
            printf("cannot add server to dictionary");
            return -1;
        }
    }

    return 0;
}

int rh_load_json(rh_state_t *state, const char *str) {
    struct json_object *jobj = 0;

    if(!(jobj = json_tokener_parse(str))) {
        printf("Json parsing failed!");
        return -1;
    }

    if(!json_object_is_type(jobj, json_type_object)) {
        printf("config.json must be a JSON object!");
        return -1;
    }

    int err = 0;
    json_object *cur = 0;

    if(json_object_object_get_ex(jobj, "plugins", &cur)) {
        if((err = rh_load_plugins(state, cur))) {
            return err;
        }
    }

    if(json_object_object_get_ex(jobj, "interfaces", &cur)) {
        if((err = rh_load_interfaces(state, cur))) {
            return err;
        }
    }

    if(json_object_object_get_ex(jobj, "tls", &cur)) {
        if((err = rh_load_tls(state, cur))) {
            return err;
        }
    }

    if(json_object_object_get_ex(jobj, "servers", &cur)) {
        if((err = rh_load_servers(state, cur))) {
            return err;
        }
    }

    return 0;
}