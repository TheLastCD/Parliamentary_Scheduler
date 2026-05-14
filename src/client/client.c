#include <client/client.h>
#include <msg/msg.h>
#include <protocols/internal/ping.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CLIENT_QUEUE_MAX 16

static struct {
    cltMeta meta;
    uint16_t client_id;
    bool running;
    const srvMeta *next_server;
    Msg *queue[CLIENT_QUEUE_MAX];
    size_t queue_size;
    bool ping_response_sent;
} client_ctx = {0};

static Msg *copy_msg(const Msg *msg) {
    if (!msg) return NULL;

    Msg *copy = malloc(sizeof(*copy));
    if (!copy) return NULL;

    *copy = *msg;
    if (msg->hdr.BodyLen > 0 && msg->msg_buff) {
        copy->msg_buff = malloc(msg->hdr.BodyLen);
        if (!copy->msg_buff) {
            free(copy);
            return NULL;
        }
        memcpy(copy->msg_buff, msg->msg_buff, msg->hdr.BodyLen);
    } else {
        copy->msg_buff = NULL;
    }

    return copy;
}

static int process_packet(const Msg *msg) {
    if (!msg) return -1;
    printf("Client %u processing packet: Seq=%u, Localref=%u, BodyType=%u\n",
           client_ctx.client_id,
           msg->hdr.SeqNum,
           msg->hdr.Localref,
           msg->bdy.BodyType);

    if (msg->bdy.BodyType == bdy_PING) {
        // Create ping response
        if (create_ping_response(&msg->bdy) == 0) {
            client_ctx.ping_response_sent = true;
            printf("Client %u sent ping response\n", client_ctx.client_id);
        }
    }

    return 0;
}

int start_client(const cltMeta *client_meta, uint16_t client_id) {
    if (!client_meta || client_ctx.running) return -1;

    client_ctx.meta = *client_meta;
    client_ctx.client_id = client_id;
    client_ctx.running = true;
    client_ctx.next_server = &client_ctx.meta.serverMeta;
    client_ctx.queue_size = 0;
    client_ctx.ping_response_sent = false;

    return 0;
}

int kill_client(void) {
    if (!client_ctx.running) return -1;

    for (size_t i = 0; i < client_ctx.queue_size; ++i) {
        if (client_ctx.queue[i]) {
            free_msg(client_ctx.queue[i]);
            free(client_ctx.queue[i]);
            client_ctx.queue[i] = NULL;
        }
    }
    client_ctx.queue_size = 0;
    client_ctx.running = false;
    client_ctx.next_server = NULL;
    return 0;
}

int add_to_queue(const Msg *msg) {
    if (!client_ctx.running || !msg) return -1;
    if (client_ctx.queue_size >= CLIENT_QUEUE_MAX) return -1;

    Msg *copy = copy_msg(msg);
    if (!copy) return -1;

    client_ctx.queue[client_ctx.queue_size++] = copy;
    return 0;
}

const srvMeta *route_msg(const Msg *msg) {
    if (!client_ctx.running || !msg) return NULL;

    if (msg->hdr.Localref == client_ctx.client_id) {
        process_packet(msg);
        return NULL;
    }

    if (client_ctx.next_server && client_ctx.next_server->ServerRole != srv_OBSERVER) {
        return client_ctx.next_server;
    }

    return NULL;
}

int receive_msg(const uint8_t *packet, size_t len) {
    if (!client_ctx.running || !packet || len == 0) return -1;

    Msg decoded = {0};
    if (!decode_msg(&decoded, packet, len)) {
        return -1;
    }

    const srvMeta *next = route_msg(&decoded);
    int result = 0;
    if (next) {
        result = add_to_queue(&decoded);
        if (result != 0) {
            free_msg(&decoded);
            return -1;
        }
        result = 1;
    }

    free_msg(&decoded);
    return result;
}

bool client_ping_response_sent(void) {
    return client_ctx.ping_response_sent;
}
