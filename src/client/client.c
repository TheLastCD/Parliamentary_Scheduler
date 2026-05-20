#include <client/client.h>
#include <callbacks/callback_manager.h>
#include <msg/msg.h>
#include <protocols/internal/ping.h>
#include <scheduler/tape.h>
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
    tape_machine tape;
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
        printf("[Client 0x%02X] Processing packet: Seq=%u, Localref=%u, BodyType=%u\n",
            (unsigned)client_ctx.client_id,
            msg->hdr.SeqNum,
            msg->hdr.Localref,
            msg->bdy.BodyType);

    if (msg->bdy.BodyType == bdy_PING) {
        // Create ping response
        if (create_ping_response(&msg->bdy) == 0) {
            client_ctx.ping_response_sent = true;
            printf("[Client 0x%02X] Sent ping response\n", (unsigned)client_ctx.client_id);
        }
    }

    if (cb_has_callbacks_for_type(msg->bdy.BodyType)) {
        cb_trigger(msg);
    }

    return 0;
}

int start_client(const cltMeta *client_meta, uint16_t client_id) {
    if (!client_meta || client_ctx.running) return -1;

    cb_manager_init();
    client_ctx.meta = *client_meta;
    client_ctx.client_id = client_id;
    client_ctx.running = true;
    client_ctx.next_server = &client_ctx.meta.serverMeta;
    client_ctx.queue_size = 0;
    client_ctx.ping_response_sent = false;
    memset(&client_ctx.tape, 0, sizeof(client_ctx.tape));

    return 0;
}

int kill_client(void) {
    if (!client_ctx.running) return -1;

    for (int i = 0; i < MAX_TAPE_LENGTH; ++i) {
        if (client_ctx.tape.tape[i].msg) {
            free_msg(client_ctx.tape.tape[i].msg);
            free(client_ctx.tape.tape[i].msg);
            client_ctx.tape.tape[i].msg = NULL;
            client_ctx.tape.tape[i].hdr = NULL;
            client_ctx.tape.tape[i].fluff = 0;
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

    if (tape_enqueue_msg(&client_ctx.tape, copy) < 0) {
        free_msg(copy);
        free(copy);
        return -1;
    }

    client_ctx.queue_size++;
        printf("[Client 0x%02X] Scheduled message: Seq=%u, Priority=%u, Localref=%u\n",
            (unsigned)client_ctx.client_id,
            copy->hdr.SeqNum,
            copy->hdr.PriorityRequested,
            copy->hdr.Localref);
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

int client_dispatch_next(void) {
    if (!client_ctx.running) return -1;

    Msg *msg = tape_take_head(&client_ctx.tape);
    if (!msg) return -1;

        printf("[Client 0x%02X] Dispatching scheduled message: Seq=%u, Priority=%u, Localref=%u, BodyType=%u\n",
            (unsigned)client_ctx.client_id,
            msg->hdr.SeqNum,
            msg->hdr.PriorityRequested,
            msg->hdr.Localref,
            msg->bdy.BodyType);

    free_msg(msg);
    free(msg);
    if (client_ctx.queue_size > 0) {
        client_ctx.queue_size--;
    }
    return 0;
}

size_t client_scheduled_count(void) {
    return client_ctx.queue_size;
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
    } else {
        result = process_packet(&decoded);
        if (cb_has_callbacks_for_type(decoded.bdy.BodyType)) {
            cb_trigger(&decoded);
        }
    }

    free_msg(&decoded);
    return result;
}

bool client_ping_response_sent(void) {
    return client_ctx.ping_response_sent;
}
