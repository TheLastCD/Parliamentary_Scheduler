#ifndef SCHEDULER_TAPE_H
#define SCHEDULER_TAPE_H

#include <msg/header.h>
#include <stdint.h>

#define MAX_TAPE_LENGTH 16


typedef struct{
	msgHeader *hdr;
	uint8_t fluff;
} tape_entry;


typedef struct{
	tape_entry tape[MAX_TAPE_LENGTH];
}tape_machine;



int act_on_head(tape_machine *tape);
int progress_head(tape_machine *tape);

#endif // SCHEDULER_TAPE_H

