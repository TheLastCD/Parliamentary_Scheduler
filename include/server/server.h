#include <stdbool.h>
#include <stdint.h>

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


typedef enum {
    srv_PRIME,
    srv_SEC,
    srv_MEMBER,
    srv_OBSERVER,
} srv_role;

typedef struct {
    srv_role ServerRole;
    uint8_t ServerCommAdv;
} srvMeta;

typedef struct {
    srvMeta ServerMetaTag;
    uint16_t ServerPort;
} srvSock;

// server state.
typedef struct {
    srvMeta meta;
    uint16_t port;
    int listen_fd;
    bool running;
} srvContext;

int srv_StartListen(void);
int srv_StopListen(void);
int srv_InitServer(srvContext *ctx, const srvSock *config);
int srv_AcceptConnection(int listen_fd);
void srv_SetGlobalContext(srvContext *ctx);


#endif 
