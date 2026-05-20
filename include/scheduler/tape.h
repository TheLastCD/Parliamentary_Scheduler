#ifndef SCHEDULER_TAPE_H
#define SCHEDULER_TAPE_H

#include <msg/msg.h>
#include <stdint.h>

#define MAX_TAPE_LENGTH 16


typedef struct{
    Msg *msg;
	msgHeader *hdr;
	uint8_t fluff;
} tape_entry;


typedef struct{
	tape_entry tape[MAX_TAPE_LENGTH];
} tape_machine;


int act_on_head(tape_machine *tape);
int progress_head(tape_machine *tape);
int tape_enqueue_msg(tape_machine *tape, Msg *msg);
Msg *tape_take_head(tape_machine *tape);

#endif // SCHEDULER_TAPE_H

