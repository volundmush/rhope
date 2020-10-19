//
// Created by volund on 10/18/20.
//

#include "config.h"

int rh_load_plugins(rhope_state_t *state, json_object *jobj) {
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
        uv_lib_t *plugin = calloc(1, sizeof(uv_lib_t));
        if(plugin == 0) {
            printf("could not allocate memory for plugin");
            return -1;
        }

        char *filename = calloc(json_object_get_string_len(entry)+ 1 + 4, 1);
        strcat(filename, json_object_get_string(entry));
        strcat(filename, ".so");

        int err = uv_dlopen(filename, plugin);
    }

}

int rh_load_interfaces(rhope_state_t *state, json_object *jobj) {
    json_object_object_foreach(jobj, key, val) {

    }
}

int rh_load_tls(rhope_state_t *state, json_object *jobj) {
    json_object_object_foreach(jobj, key, val) {

    }
}

int rh_load_servers(rhope_state_t *state, json_object *jobj) {
    json_object_object_foreach(jobj, key, val) {

    }
}

int rh_load_json(rhope_state_t *state, const char *str) {
    struct json_object *jobj = json_tokener_parse(str);

    if(jobj == 0) {
        printf("Json parsing failed!");
        return -1;
    }

    if(!json_object_is_type(jobj, json_type_object)) {
        printf("config.json must be a JSON object!");
        return -1;
    }

    int err = 0;

    json_object *cur = 0;

    if(json_object_object_get_ex(jobj, "plugins", &cur) != 0) {
        err = rh_load_plugins(state, cur);
        if(err) {
            return err;
        }
    }

    if(json_object_object_get_ex(jobj, "interfaces", &cur) != 0) {
        err = rh_load_interfaces(state, cur);
        if(err) {
            return err;
        }
    }

    if(json_object_object_get_ex(jobj, "tls", &cur) != 0) {
        err = rh_load_tls(state, cur);
        if(err) {
            return err;
        }
    }

    if(json_object_object_get_ex(jobj, "servers", &cur) != 0) {
        err = rh_load_servers(state, cur);
        if(err) {
            return err;
        }
    }

    return 0;
}