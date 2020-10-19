//
// Created by volund on 10/18/20.
//

#include "rhope.h"

rh_kvnode_t *rh_dict_get_node(rh_dict_t *holder, char *in_key) {
    rh_kvnode_t *cur = holder->head;
    while(cur != 0) {
        if(strcmp(cur->key, in_key) == 0) {
            return cur;
        } else {
            cur = cur->next;
        }
    }
    return 0;
}

void *rh_dict_get_value(rh_dict_t *holder, char *in_key) {
    // this will return a nullptr if nothing is found...
    rh_kvnode_t *found = rh_dict_get_node(holder, in_key);
    if(found == 0) {
        return 0;
    }
    return found->value;
}

int rh_dict_get_or_create(rh_dict_t *holder, char *in_key, rh_kvnode_t **out) {
    rh_kvnode_t *found = rh_dict_get_node(holder, in_key);
    if(found) {
        *out = found;
        return 0;
    }
    rh_kvnode_t *new_node = malloc(sizeof(rh_kvnode_t));

    if(new_node == 0) {
        // malloc failed!
        return -1;
    }

    new_node->key = calloc(strlen(in_key)+1, 1);
    if(new_node->key == 0) {
        // calloc failed!
        return -1;
    }
    strcpy(new_node->key, in_key);

    if(holder->head != 0) {
        holder->tail->next = new_node;
        holder->tail = new_node;
    } else {
        holder->head = new_node;
        holder->tail = new_node;
    }
    *out = new_node;
    return 0;
}

// Inserts - or replaces an existing - key with the given value into *holder.
int rh_dict_insert(rh_dict_t *holder, char *in_key, void *in_value) {
    rh_kvnode_t *node = 0;
    int err = rh_dict_get_or_create(holder, in_key, &node);
    if(err) {
        return err;
    }

    node->value = in_value;
    return 0;
}