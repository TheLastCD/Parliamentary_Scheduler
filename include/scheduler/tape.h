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




