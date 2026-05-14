#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <server/server.h>

int test_server_init(void) {
    srvSock config = {
        .ServerMetaTag = {
            .ServerRole = srv_PRIME,
            .ServerCommAdv = 0x01
        },
        .ServerPort = 8080
    };

    srvContext ctx;
    int result = srv_InitServer(&ctx, &config);
    assert(result == 0);
    assert(ctx.meta.ServerRole == srv_PRIME);
    assert(ctx.port == 8080);
    assert(ctx.listen_fd == -1);
    assert(ctx.running == false);

    return 1;
}

int test_server_start_stop(void) {
    srvSock config = {
        .ServerMetaTag = {
            .ServerRole = srv_MEMBER,
            .ServerCommAdv = 0x02
        },
        .ServerPort = 8081
    };

    srvContext ctx;
    srv_InitServer(&ctx, &config);
    srv_SetGlobalContext(&ctx);

    int start_result = srv_StartListen();
    assert(start_result == 0);
    assert(ctx.running == true);
    assert(ctx.listen_fd >= 0);

    int stop_result = srv_StopListen();
    assert(stop_result == 0);
    assert(ctx.running == false);
    assert(ctx.listen_fd == -1);

    srv_SetGlobalContext(NULL);
    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing server implementation\n");
    assert(test_server_init() == 1);
    assert(test_server_start_stop() == 1);
    printf("--server tests successful\n");
    return 0;
}
#endif
