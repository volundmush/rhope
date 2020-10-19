//
// Created by volund on 10/18/20.
//

#include "rhope.h"
#include <json-c/json.h>

int rh_load_json(rhope_state_t *state, const char *str) {
    struct json_object *jobj = json_tokener_parse(str);

    if(jobj == 0) {
        printf("Json parsing failed!");
        return -1;
    }


}

int rh_load_plugins(rhope_state_t *state) {

}

int rh_load_interfaces(rhope_state_t *state) {

}

