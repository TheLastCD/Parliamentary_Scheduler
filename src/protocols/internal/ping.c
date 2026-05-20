#include <protocols/internal/ping.h>
#include <msg/msg.h>
#include <stdlib.h>
#include <string.h>

// Internal static buffers for the last-created payloads
static uint8_t ping_payload_buf[1] = {0};
static size_t ping_payload_len = 0;

static uint8_t ping_response_buf[1] = {0};
static size_t ping_response_len = 0;

int create_ping(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_PING) return -1;

    // Build a minimal ping payload: single byte 0x90
    ping_payload_buf[0] = 0x90;
    ping_payload_len = 1;

    return 0;
}

int create_ping_response(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_PING) return -1;

    // Build a minimal ping response payload: single byte 0x01
    ping_response_buf[0] = 0x01;
    ping_response_len = 1;

    return 0;
}

const uint8_t *ping_get_payload(size_t *out_len) {
    if (out_len) *out_len = ping_payload_len;
    return ping_payload_len ? ping_payload_buf : NULL;
}

const uint8_t *ping_get_response_payload(size_t *out_len) {
    if (out_len) *out_len = ping_response_len;
    return ping_response_len ? ping_response_buf : NULL;
}

int ping_build_confirm(uint8_t seqnum, uint8_t *outbuf, size_t buf_len, size_t *out_len) {
    if (!outbuf || buf_len < 2) return -1;
    outbuf[0] = seqnum;
    outbuf[1] = 0x01;
    if (out_len) *out_len = 2;
    return 0;
}

int create_ping_message(Msg *out, uint8_t requester, uint8_t priority, uint8_t seq, uint8_t localref, bdr_ret return_type) {
    if (!out) return -1;

    // Ensure internal payload initialized
    msgBody bdy = { .BodyType = bdy_PING, .ReturnType = return_type };
    if (create_ping(&bdy) != 0) return -1;

    out->hdr.Requester = requester;
    out->hdr.PriorityRequested = (hdr_reqpriority)priority;
    out->hdr.SeqNum = seq;
    out->hdr.Localref = localref;
    out->hdr.BodyLen = (uint8_t)ping_payload_len;

    out->bdy = bdy;

    if (ping_payload_len > 0) {
        out->msg_buff = malloc(ping_payload_len);
        if (!out->msg_buff) return -1;
        memcpy(out->msg_buff, ping_payload_buf, ping_payload_len);
    } else {
        out->msg_buff = NULL;
    }

    return 0;
}
