#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include <client/client.h>
#include <msg/msg.h>

static uint8_t make_packet(uint8_t requester, uint8_t priority, uint8_t seq, uint8_t localref, uint8_t bodytype, uint8_t returntype, const uint8_t *payload, size_t payload_len, uint8_t *buffer, size_t buf_len) {
    Msg msg = {0};
    msg.hdr.Requester = requester;
    msg.hdr.PriorityRequested = priority;
    msg.hdr.SeqNum = seq;
    msg.hdr.Localref = localref;
    msg.hdr.BodyLen = (uint8_t)payload_len;
    msg.bdy.BodyType = bodytype;
    msg.bdy.ReturnType = returntype;
    msg.msg_buff = (MsgBuffer)payload;

    int result = encode_msg(&msg, buffer, buf_len);
    assert(result == 1);
    return result;
}

int test_client_routing(void) {
    cltMeta client_meta = {
        .cltRole = clt_MEMBER,
        .serverMeta = {
            .ServerRole = srv_SEC,
            .ServerCommAdv = 0x42,
        },
    };

    assert(start_client(&client_meta, 0x11) == 0);

    uint8_t payload[] = {0xDE, 0xAD};
    uint8_t packet[24 + sizeof(payload)];
    make_packet(0x01, hdr_HIGH, 0x02, 0x22, bdy_READ, bdy_CONFIRM, payload, sizeof(payload), packet, sizeof(packet));

    int result = receive_msg(packet, sizeof(packet));
    assert(result == 1); // should route to next server because Localref != client id

    assert(kill_client() == 0);
    return 1;
}

int test_client_process(void) {
    cltMeta client_meta = {
        .cltRole = clt_MEMBER,
        .serverMeta = {
            .ServerRole = srv_SEC,
            .ServerCommAdv = 0x42,
        },
    };

    assert(start_client(&client_meta, 0x33) == 0);

    uint8_t payload[] = {0xBE, 0xEF};
    uint8_t packet[24 + sizeof(payload)];
    make_packet(0x01, hdr_LOW, 0x05, 0x33, bdy_PING, bdy_CONFIRM, payload, sizeof(payload), packet, sizeof(packet));

    int result = receive_msg(packet, sizeof(packet));
    assert(result == 0); // processed locally

    assert(kill_client() == 0);
    return 1;
}

int test_client_process_from_server(void) {
    cltMeta client_meta = {
        .cltRole = clt_MEMBER,
        .serverMeta = {
            .ServerRole = srv_PRIME,
            .ServerCommAdv = 0x99,
        },
    };

    assert(start_client(&client_meta, 0x44) == 0);

    uint8_t payload[] = {0xCA, 0xFE, 0xBA, 0xBE};
    uint8_t packet[24 + sizeof(payload)];
    make_packet(0x02, hdr_MED, 0x10, 0x44, bdy_WRITE, bdy_MSG, payload, sizeof(payload), packet, sizeof(packet));

    int result = receive_msg(packet, sizeof(packet));
    assert(result == 0); // server delivered packet intended for this client and it was processed

    assert(kill_client() == 0);
    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing client routing and packet processing\n");
    assert(test_client_routing() == 1);
    assert(test_client_process() == 1);
    assert(test_client_process_from_server() == 1);
    printf("--client tests successful\n");
    return 0;
}
#endif
