//
// Created by volund on 10/18/20.
//

#ifndef RHOPE_CONFIG_H
#define RHOPE_CONFIG_H

#include "rhope.h"
#include <json-c/json.h>

int rh_load_plugins(rh_state_t *state, json_object *jobj);

int rh_load_interfaces(rh_state_t *state, json_object *jobj);

int rh_load_tls(rh_state_t *state, json_object *jobj);

int rh_load_servers(rh_state_t *state, json_object *jobj);

int rh_load_json(rh_state_t *state, const char *str);

#endif //RHOPE_CONFIG_H
