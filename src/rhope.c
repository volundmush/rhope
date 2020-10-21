//
// Created by volund on 10/18/20.
//

#include "rhope.h"

int rh_strcpy(char **in_dest, const char *in_src) {
    *in_dest = calloc(strlen(in_src)+1, 1);
    if(*in_dest == 0) {
        return -1;
    }
    strcpy(*in_dest, in_src);
    return 0;
}


rh_kvnode_t *rh_dict_get_node(rh_dict_t *in_dict, char *in_key) {
    rh_kvnode_t *cur = in_dict->head;
    while(cur != 0) {
        if(strcmp(cur->key, in_key) == 0) {
            return cur;
        } else {
            cur = cur->next;
        }
    }
    return 0;
}

void *rh_dict_get_value(rh_dict_t *in_dict, char *in_key) {
    // this will return a nullptr if nothing is found...
    rh_kvnode_t *found = rh_dict_get_node(in_dict, in_key);
    if(found == 0) {
        return 0;
    }
    return found->value;
}

int rh_dict_get_or_create(rh_dict_t *in_dict, char *in_key, rh_kvnode_t **in_out) {
    rh_kvnode_t *found = rh_dict_get_node(in_dict, in_key);
    if(found) {
        *in_out = found;
        return 0;
    }
    rh_kvnode_t *new_node = malloc(sizeof(rh_kvnode_t));

    if(new_node == 0) {
        // malloc failed!
        return -1;
    }

    if(rh_strcpy(&new_node->key, in_key)) {
        return -1;
    }

    if(in_dict->head != 0) {
        in_dict->tail->next = new_node;
        in_dict->tail = new_node;
    } else {
        in_dict->head = new_node;
        in_dict->tail = new_node;
    }
    *in_out = new_node;
    return 0;
}

// Inserts - or replaces an existing - key with the given value into *holder.
int rh_dict_insert(rh_dict_t *in_dict, char *in_key, void *in_value) {
    rh_kvnode_t *node = 0;
    int err = rh_dict_get_or_create(in_dict, in_key, &node);
    if(err) {
        return err;
    }

    node->value = in_value;
    return 0;
}