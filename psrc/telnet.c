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

#define TELNET_MTTS_ANSI 0x1
#define TELNET_MTTS_VT100 0x2
#define TELNET_MTTS_UTF8 0x4
#define TELNET_MTTS_256COLOR 0x8
#define TELNET_MTTS_MOUSETRACKING 0x16
#define TELNET_MTTS_OSCCOLORPALETTE 0x32
#define TELNET_MTTS_SCREENREADER 0x64
#define TELNET_MTTS_PROXY 0x128
#define TELNET_MTTS_TRUECOLOR 0x256
#define TELNET_MTTS_MNES 0x512
#define TELNET_MTTS_MSLP 0x1024

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

typedef struct rh_mudtelnet_telopt_s {
    int option;
    int negotiate;
} rh_mudtelnet_telopt_t;

static const rh_mudtelnet_telopt_t mudtelstart[] = {
        {TELNET_TELOPT_LINEMODE, TELNET_DO},
        {TELNET_TELOPT_SGA, TELNET_WILL},
        {TELNET_TELOPT_NAWS, TELNET_DO},
        {TELNET_TELOPT_TTYPE, TELNET_DO},
        {TELNET_TELOPT_MCCP2, TELNET_WILL},
        {TELNET_TELOPT_MSSP, TELNET_WILL},
        {TELNET_TELOPT_MSDP, TELNET_WILL},
        {TELNET_TELOPT_GMCP, TELNET_WILL},
        {TELNET_TELOPT_EOR, TELNET_WILL},
        //{TELNET_TELOPT_MXP, TELNET_WILL}
};

typedef struct rh_mudtelnet_charbuffer_s {
    char *buffer;
    unsigned long length;
    unsigned long capacity;
    unsigned long cursor;
} rh_mudtelnet_charbuffer_t;

typedef struct rh_mudtelnet_naws_s {
    int width;
    int height;
} rh_mudtelnet_naws_t;

typedef struct rh_mudtelnet_ttype_s {
    int handshakes;
    char *client_name;
    char *client_version;
    char *terminal_type;
    int mtts_flags;
} rh_mudtelnet_ttype_t;

typedef struct rh_mudtelnet_appdata_s {
    uv_tcp_t *handle;
    uv_tcp_t *server;
    telnet_t *telnet;
    rh_mudtelnet_naws_t naws;
    rh_mudtelnet_ttype_t ttype;
    rh_mudtelnet_charbuffer_t cmd_buffer;
    // if EOR is set, IAC EOR will be used for prompts. else, IAC GA will.
    int eor;
} rh_mudtelnet_appdata_t;

int rh_mudtelnet_charbuffer_init(rh_mudtelnet_charbuffer_t *buf) {
    buf->capacity = 8096 * 4;
    buf->cursor = 0;
    if(!(buf->buffer = calloc(buf->capacity, 1))) {
        return -1;
    }
    return 0;
}

int rh_mudtelnet_appdata_init(rh_mudtelnet_appdata_t *data) {
    if(rh_mudtelnet_charbuffer_init(&data->cmd_buffer)) {
        return -1;
    }
    return 0;
}

void rh_mudtelnet_event_ev_data(telnet_t *telnet, telnet_event_t *ev, void *user_data) {
    rh_mudtelnet_appdata_t *appdata = (rh_mudtelnet_appdata_t*)user_data;
    fwrite(ev->data.buffer, 1, ev->data.size, stdout);
    printf("received data!");
}

void rh_mudtelnet_event_ev_send(telnet_t *telnet, telnet_event_t *ev, void *user_data) {
    rh_mudtelnet_appdata_t *appdata = (rh_mudtelnet_appdata_t*)user_data;
    uv_write_t *write = calloc(1, sizeof(uv_write_t));
    uv_buf_t *buf = calloc(1, sizeof(uv_buf_t));
    buf->base = calloc(1, ev->data.size);
    buf->len = ev->data.size;
    memcpy(buf->base, ev->data.buffer, ev->data.size);
    rh_server_t *server = appdata->server->data;
    uv_write(write, (uv_stream_t*)appdata->handle, buf, 1, server->protocol->on_write);
}

void rh_mudtelnet_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data) {

    switch(ev->type) {
        case TELNET_EV_DATA:
            // This is commands typed by the user!
            rh_mudtelnet_event_ev_data(telnet, ev, user_data);
            break;
        case TELNET_EV_SEND:
            // This is text being sent to the user!
            rh_mudtelnet_event_ev_send(telnet, ev, user_data);
            break;
        case TELNET_EV_IAC:
            // We received a simple IAC command... what to do with it?
            break;
        case TELNET_EV_WILL:
            break;
        case TELNET_EV_DO:
            break;
        case TELNET_EV_WONT:
            break;
        case TELNET_EV_DONT:
            break;
        case TELNET_EV_SUBNEGOTIATION:
            break;
        case TELNET_EV_COMPRESS:
            break;
        case TELNET_EV_ZMP:
            break;
        case TELNET_EV_TTYPE:
            break;
        case TELNET_EV_ENVIRON:
            break;
        case TELNET_EV_MSSP:
            break;
        case TELNET_EV_WARNING:
            break;
        case TELNET_EV_ERROR:
            break;
    }

}

void rh_mudtelnet_connect_cb(uv_connect_t *req, int status) {
    // This isn't used... yet...
}

void rh_mudtelnet_connection_cb(uv_stream_t *server, int status) {
    rh_server_t *rs = (rh_server_t*)server->data;

    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_tcp_t *client = 0;
    if(!(client = (uv_tcp_t*) calloc(1, sizeof(uv_tcp_t)))) {
        printf("alloc error for client");
        return;
    }

    uv_tcp_init(server->loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        rh_mudtelnet_appdata_t *ud = calloc(1, sizeof(rh_mudtelnet_appdata_t));
        if(!(ud && !rh_mudtelnet_appdata_init(ud))) {
            printf("alloc error for appdata");
            return;
        }
        ud->handle = client;
        ud->server = (uv_tcp_t*)server;
        telnet_t *tel = telnet_init((const telnet_telopt_t *)&mudtelopts, rh_mudtelnet_event_handler, 0, (void*)ud);
        ud->telnet = tel;
        client->data = (void*)ud;

        for(int i = 0; i <=8; i++) {
            telnet_negotiate(tel, mudtelstart[i].negotiate, mudtelstart[i].option);
        }
        uv_read_start((uv_stream_t*) client, rs->protocol->on_alloc, rs->protocol->on_read);
    }
    else {
        uv_close((uv_handle_t*) client, rs->protocol->on_close);
    }

}

void rh_mudtelnet_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = calloc(suggested_size, 1);
    buf->len = suggested_size;
}

void rh_mudtelnet_read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    rh_mudtelnet_appdata_t *appdata = (rh_mudtelnet_appdata_t*)stream->data;
    telnet_recv(appdata->telnet, buf->base, nread);
    free(buf->base);
}

void rh_mudtelnet_write_cb(uv_write_t *req, int status) {

}

void rh_mudtelnet_close_cb(uv_handle_t *handle) {

}



void rh_mudtelnet_shutdown_cb(uv_shutdown_t *req, int status) {

}

rh_protocol_t mudtelnet = {"mudtelnet",
                           &rh_mudtelnet_connect_cb,
                           &rh_mudtelnet_connection_cb,
                           &rh_mudtelnet_read_cb,
                           &rh_mudtelnet_write_cb,
                           &rh_mudtelnet_close_cb,
                           &rh_mudtelnet_alloc_cb,
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

