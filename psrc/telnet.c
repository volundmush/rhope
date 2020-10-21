//
// Created by volund on 10/18/20.
//

#include <stddef.h>
#include <libtelnet.h>
#include "rhope.h"

// The following are special MUD specific protocols.

// MNES: Mud New-Environ standard
#define TELNET_TELOPT_MNES 39

// MUD eXtension Protocol
// NOTE: Disabled due to too many issues with it.
#define TELNET_TELOPT_MXP 91

// GMCP - Generic Mud Communication Protocol
#define TELNET_TELOPT_GMCP 201

// MSDP - Mud Server Data Protocol
#define TELNET_TELOPT_MSDP 69


static const telnet_telopt_t mudtelopts[] = {
        { TELNET_TELOPT_SGA,       TELNET_WILL, TELNET_DO   },
        { TELNET_TELOPT_NAWS,      TELNET_WILL, TELNET_DO   },
        { TELNET_TELOPT_TTYPE,     TELNET_WONT, TELNET_DO   },
        //{ TELNET_TELOPT_MXP,       TELNET_WILL, TELNET_DONT },
        { TELNET_TELOPT_MSSP,      TELNET_WONT, TELNET_DO   },
        { TELNET_TELOPT_COMPRESS2, TELNET_WILL, TELNET_DONT },
        { TELNET_TELOPT_ZMP,       TELNET_WONT, TELNET_DONT },
        { TELNET_TELOPT_MSDP,      TELNET_WILL, TELNET_DO   },
        { TELNET_TELOPT_GMCP,      TELNET_WILL, TELNET_DO   },
        { TELNET_TELOPT_LINEMODE,  TELNET_WONT, TELNET_DO   },
        { TELNET_TELOPT_EOR,       TELNET_WILL, TELNET_DONT },
        { -1, 0, 0 }
};

void rh_mudtelnet_connect_cb(uv_connect_t *req, int status) {

}

void rh_mudtelnet_connection_cb(uv_stream_t *server, int status) {

}

void rh_mudtelnet_read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {

}

void rh_mudtelnet_write_cb(uv_write_t *req, int status) {

}

void rh_mudtelnet_shutdown_cb(uv_shutdown_t *req, int status) {

}

rh_protocol_t mudtelnet = {"mudtelnet",
                           &rh_mudtelnet_connect_cb,
                           &rh_mudtelnet_connection_cb,
                           &rh_mudtelnet_read_cb,
                           &rh_mudtelnet_write_cb,
                           &rh_mudtelnet_shutdown_cb
                           };

int plugin_init(rh_state_t *state) {
    rh_dict_insert(&state->protocols, mudtelnet.name, (void*)&mudtelnet);
    return 0;
}

int plugin_start(rh_state_t *state) {
    return 0;
}

int plugin_shutdown(rh_state_t *state) {
    return 0;
}

