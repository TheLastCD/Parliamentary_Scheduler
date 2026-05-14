#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <msg/msg.h>
#include <scheduler/tape.h>

static Msg make_test_message(const uint8_t *payload, size_t payload_len) {
    Msg msg = {0};
    msg.hdr.Requester = 0x01;
    msg.hdr.PriorityRequested = hdr_HIGH;
    msg.hdr.SeqNum = 0x02;
    msg.hdr.Localref = 0x10;
    msg.hdr.BodyLen = (uint8_t)payload_len;
    msg.bdy.BodyType = bdy_READ;
    msg.bdy.ReturnType = bdy_CONFIRM;
    msg.msg_buff = (MsgBuffer)payload;
    return msg;
}

int test_tape_messages(void) {
    uint8_t payload[] = {0xAA, 0xBB, 0xCC};
    Msg original = make_test_message(payload, sizeof(payload));
    uint8_t buffer[FIXED_HDR_SIZE + FIXED_BDY_SIZE + sizeof(payload)];

    int encoded = encode_msg(&original, buffer, sizeof(buffer));
    assert(encoded == 1);

    Msg decoded = {0};
    int decoded_ok = decode_msg(&decoded, buffer, sizeof(buffer));
    assert(decoded_ok == 1);
    assert(decoded.hdr.Requester == original.hdr.Requester);
    assert(decoded.hdr.PriorityRequested == original.hdr.PriorityRequested);
    assert(decoded.hdr.SeqNum == original.hdr.SeqNum);
    assert(decoded.hdr.Localref == original.hdr.Localref);
    assert(decoded.hdr.BodyLen == original.hdr.BodyLen);
    assert(decoded.bdy.BodyType == original.bdy.BodyType);
    assert(decoded.bdy.ReturnType == original.bdy.ReturnType);
    assert(decoded.msg_buff != NULL);
    assert(memcmp(decoded.msg_buff, payload, decoded.hdr.BodyLen) == 0);

    tape_machine tape = {0};
    msgHeader *entry_hdr = malloc(sizeof(msgHeader));
    assert(entry_hdr != NULL);
    *entry_hdr = decoded.hdr;
    tape.tape[0].hdr = entry_hdr;
    tape.tape[0].fluff = 0xFF;

    int head_index = act_on_head(&tape);
    assert(head_index == 0);
    assert(tape.tape[head_index].hdr == entry_hdr);
    assert(tape.tape[head_index].hdr->Requester == decoded.hdr.Requester);

    int progressed = progress_head(&tape);
    assert(progressed == 0);
    assert(tape.tape[0].hdr == NULL);
    assert(tape.tape[MAX_TAPE_LENGTH - 1].hdr == NULL);

    free(entry_hdr);
    free_msg(&decoded);
    return 1;
}

int test_tape_shift(void) {
    tape_machine tape = {0};

    msgHeader *first = malloc(sizeof(msgHeader));
    msgHeader *second = malloc(sizeof(msgHeader));
    assert(first != NULL && second != NULL);

    *first = (msgHeader){
        .Requester = 0x11,
        .PriorityRequested = hdr_LOW,
        .SeqNum = 0x22,
        .Localref = 0x33,
        .BodyLen = 0
    };
    *second = (msgHeader){
        .Requester = 0x44,
        .PriorityRequested = hdr_HIGH,
        .SeqNum = 0x55,
        .Localref = 0x66,
        .BodyLen = 0
    };

    tape.tape[1].hdr = first;
    tape.tape[2].hdr = second;

    int head_index = act_on_head(&tape);
    assert(head_index == 1);

    int progressed = progress_head(&tape);
    assert(progressed == 1);
    assert(tape.tape[0].hdr == NULL);
    assert(tape.tape[1].hdr != NULL);
    assert(tape.tape[1].hdr->Requester == second->Requester);
    assert(tape.tape[2].hdr == NULL);

    free(tape.tape[1].hdr);
    return 1;
}

int test_tape_empty(void) {
    tape_machine tape = {0};
    assert(act_on_head(&tape) == -1);
    assert(progress_head(&tape) == -1);
    return 1;
}

int test_tape_null(void) {
    assert(act_on_head(NULL) == -1);
    assert(progress_head(NULL) == -1);
    return 1;
}

int test_tape_last_entry(void) {
    tape_machine tape = {0};
    msgHeader *last = malloc(sizeof(msgHeader));
    assert(last != NULL);
    *last = (msgHeader){
        .Requester = 0x77,
        .PriorityRequested = hdr_MED,
        .SeqNum = 0x88,
        .Localref = 0x99,
        .BodyLen = 0
    };

    tape.tape[MAX_TAPE_LENGTH - 1].hdr = last;
    int head_index = act_on_head(&tape);
    assert(head_index == MAX_TAPE_LENGTH - 1);

    int progressed = progress_head(&tape);
    assert(progressed == MAX_TAPE_LENGTH - 1);
    assert(tape.tape[MAX_TAPE_LENGTH - 1].hdr == NULL);

    free(last);
    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing tape machine and message encode/decode\n");
    assert(test_tape_messages() == 1);
    assert(test_tape_shift() == 1);
    assert(test_tape_empty() == 1);
    assert(test_tape_null() == 1);
    assert(test_tape_last_entry() == 1);
    printf("--tape tests successful\n");
    return 0;
}
#endif
