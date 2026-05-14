# Tape Queuing

This document describes the tape queue implementation used in the Parliamentary Scheduler repository.

## Files
- `include/scheduler/tape.h`
- `src/scheduler/tape.c`
- `test/scheduler/test_tape.c`

## Data Structures
### tape_entry
Defined in `include/scheduler/tape.h`.
- `msgHeader *hdr` — pointer to the queued message header.
- `uint8_t fluff` — optional metadata placeholder.

### tape_machine
Defined in `include/scheduler/tape.h`.
- `tape_entry tape[MAX_TAPE_LENGTH]` — fixed-length queue of entries.
- `MAX_TAPE_LENGTH` is currently defined as `16`.

## Tape Behavior
### act_on_head(tape_machine *tape)
- Returns the index of the first non-empty queue entry.
- Scans from index `0` to `MAX_TAPE_LENGTH - 1`.
- Returns `-1` if the tape is null or no entries are present.

### progress_head(tape_machine *tape)
- Removes the first non-empty entry and shifts remaining entries up.
- If the first occupied entry is at index `i`, entries from `i+1` onward are memmoved into place starting at `i`.
- The tail entry is cleared after shifting.
- If only the final slot contains data, it is cleared directly.
- Returns the removed index or `-1` when no entry exists.

## Notes
- The implementation treats a `NULL` `hdr` pointer as an empty slot.
- `fluff` is preserved on existing entries during shifting, but cleared when a slot becomes empty.
- Tests use dynamically allocated `msgHeader` objects and free them after progress.
