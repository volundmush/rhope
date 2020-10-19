//
// Created by volund on 10/18/20.
//

#ifndef RHOPE_CONFIG_H
#define RHOPE_CONFIG_H

#include "rhope.h"
#include <json-c/json.h>

int rh_load_plugins(rhope_state_t *state, json_object *jobj);

int rh_load_interfaces(rhope_state_t *state, json_object *jobj);

int rh_load_tls(rhope_state_t *state, json_object *jobj);

int rh_load_servers(rhope_state_t *state, json_object *jobj);

int rh_load_json(rhope_state_t *state, const char *str);

#endif //RHOPE_CONFIG_H
