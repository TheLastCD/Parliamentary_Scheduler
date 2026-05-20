#include <scheduler/tape.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int find_insert_index(tape_machine *tape, Msg *msg) {
    if (!tape || !msg) return -1;

    int free_slot = -1;
    for (int i = 0; i < MAX_TAPE_LENGTH; i++) {
        if (tape->tape[i].hdr == NULL) {
            free_slot = i;
            break;
        }
    }

    if (free_slot < 0) {
        return -1;
    }

    int insert_index = free_slot;
    for (int i = 0; i < free_slot; i++) {
        if (tape->tape[i].hdr == NULL) continue;

        if (msg->hdr.PriorityRequested > tape->tape[i].hdr->PriorityRequested ||
           (msg->hdr.PriorityRequested == tape->tape[i].hdr->PriorityRequested && msg->hdr.SeqNum < tape->tape[i].hdr->SeqNum)) {
            insert_index = i;
            break;
        }
    }

    return insert_index;
}

int act_on_head(tape_machine *tape) {
    if (!tape) return -1;
    
    for (int i = 0; i < MAX_TAPE_LENGTH; i++) {
        if (tape->tape[i].hdr != NULL) {
            return i;
        }
    }
    return -1;
}

int progress_head(tape_machine *tape) {
    if (!tape) return -1;

    for (int i = 0; i < MAX_TAPE_LENGTH - 1; i++) {
        if (tape->tape[i].hdr != NULL) {
            memmove(&tape->tape[i], &tape->tape[i+1], sizeof(tape_entry) * (MAX_TAPE_LENGTH - i - 1));
            tape->tape[MAX_TAPE_LENGTH - 1].msg = NULL;
            tape->tape[MAX_TAPE_LENGTH - 1].hdr = NULL;
            tape->tape[MAX_TAPE_LENGTH - 1].fluff = 0;
            return i;
        }
    }

    if (tape->tape[MAX_TAPE_LENGTH - 1].hdr != NULL) {
        tape->tape[MAX_TAPE_LENGTH - 1].msg = NULL;
        tape->tape[MAX_TAPE_LENGTH - 1].hdr = NULL;
        tape->tape[MAX_TAPE_LENGTH - 1].fluff = 0;
        return MAX_TAPE_LENGTH - 1;
    }

    return -1;
}

int tape_enqueue_msg(tape_machine *tape, Msg *msg) {
    if (!tape || !msg) return -1;

    int insert_index = find_insert_index(tape, msg);
    if (insert_index < 0) return -1;

    int free_slot = -1;
    for (int i = 0; i < MAX_TAPE_LENGTH; i++) {
        if (tape->tape[i].hdr == NULL) {
            free_slot = i;
            break;
        }
    }

    for (int j = free_slot; j > insert_index; j--) {
        tape->tape[j] = tape->tape[j - 1];
    }

    tape->tape[insert_index].msg = msg;
    tape->tape[insert_index].hdr = &msg->hdr;
    tape->tape[insert_index].fluff = 0;
    return insert_index;
}

Msg *tape_take_head(tape_machine *tape) {
    if (!tape) return NULL;

    int head_index = act_on_head(tape);
    if (head_index < 0) return NULL;

    Msg *msg = tape->tape[head_index].msg;
    progress_head(tape);
    return msg;
}