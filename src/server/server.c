#include <server/server.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static srvContext *global_ctx = NULL;

int srv_InitServer(srvContext *ctx, const srvSock *config) {
    if (!ctx || !config) return -1;

    ctx->meta = config->ServerMetaTag;
    ctx->port = config->ServerPort;
    ctx->listen_fd = -1;
    ctx->running = false;

    return 0;
}

int srv_StartListen(void) {
    if (!global_ctx) return -1;

    struct sockaddr_in address;
    int opt = 1;

    if ((global_ctx->listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    if (setsockopt(global_ctx->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        close(global_ctx->listen_fd);
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(global_ctx->port);

    if (bind(global_ctx->listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(global_ctx->listen_fd);
        return -1;
    }

    if (listen(global_ctx->listen_fd, 3) < 0) {
        perror("Listen failed");
        close(global_ctx->listen_fd);
        return -1;
    }

    global_ctx->running = true;
    return 0;
}

int srv_StopListen(void) {
    if (!global_ctx || !global_ctx->running) return -1;

    close(global_ctx->listen_fd);
    global_ctx->listen_fd = -1;
    global_ctx->running = false;
    return 0;
}

int srv_AcceptConnection(int listen_fd) {
    if (listen_fd < 0) return -1;

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    int new_socket = accept(listen_fd, (struct sockaddr *)&address, &addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        return -1;
    }

    return new_socket;
}

// Helper to set global context for testing
void srv_SetGlobalContext(srvContext *ctx) {
    global_ctx = ctx;
}
