#include <scheduler/tape.h>
#include <stdlib.h>
#include <string.h>

int act_on_head(tape_machine *tape) {
    if (!tape) return -1;
    
    // Find the first non-empty entry
    for (int i = 0; i < MAX_TAPE_LENGTH; i++) {
        if (tape->tape[i].hdr != NULL) {
            // Process the head entry
            // For now, just return the index
            return i;
        }
    }
    
    return -1; // No entries to act on
}

int progress_head(tape_machine *tape) {
    if (!tape) return -1;
    
    // Find the first non-empty entry and remove it
    for (int i = 0; i < MAX_TAPE_LENGTH - 1; i++) {
        if (tape->tape[i].hdr != NULL) {
            // Move all subsequent entries up
            memmove(&tape->tape[i], &tape->tape[i+1], 
                   sizeof(tape_entry) * (MAX_TAPE_LENGTH - i - 1));
            // Clear the last entry
            tape->tape[MAX_TAPE_LENGTH - 1].hdr = NULL;
            tape->tape[MAX_TAPE_LENGTH - 1].fluff = 0;
            return i;
        }
    }
    
    // If only the last entry has data, clear it
    if (tape->tape[MAX_TAPE_LENGTH - 1].hdr != NULL) {
        tape->tape[MAX_TAPE_LENGTH - 1].hdr = NULL;
        tape->tape[MAX_TAPE_LENGTH - 1].fluff = 0;
        return MAX_TAPE_LENGTH - 1;
    }
    
    return -1; // No entries to progress
}