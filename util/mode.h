#pragma once

enum mode_e {
    CLIENT_CONN_REQ = 1, // done~
    SERVER_LISTEN_REQ, // 
    MNGR_CONN_RES,
    MNGR_LISTEN_RES,
    CLIENT_CLOSE_REQ,
    SERVER_CLOSE_REQ,
    SERVER_NOT_REGISTED,
    ACK,
    SERVER_AQ_FULL,
    CREATE_SHM_FAILED,
    CONNECTION_CLOSED,
    LISTEN_CLOSE,
    CONNECT_CLOSE,
    LISTEN_CLOSED
};