//
// Created by volund on 10/18/20.
//

#include <stddef.h>
#include <libtelnet.h>
#include "rhope.h"

static const telnet_telopt_t mudtelopts[] = {
        { TELNET_TELOPT_ECHO,      TELNET_WILL, TELNET_DONT },
        { TELNET_TELOPT_TTYPE,     TELNET_WILL, TELNET_DONT },
        { TELNET_TELOPT_COMPRESS2, TELNET_WONT, TELNET_DO   },
        { TELNET_TELOPT_ZMP,       TELNET_WONT, TELNET_DO   },
        { TELNET_TELOPT_MSSP,      TELNET_WONT, TELNET_DO   },
        { TELNET_TELOPT_BINARY,    TELNET_WILL, TELNET_DO   },
        { TELNET_TELOPT_NAWS,      TELNET_WILL, TELNET_DONT },
        { -1, 0, 0 }
};

int plugin_init(rh_state_t *state) {
    printf("RAWR FROM MUDTELNET!");
    return 0;
}

int plugin_start(rh_state_t *state) {
    return 0;
}

int plugin_shutdown(rh_state_t *state) {
    return 0;
}

