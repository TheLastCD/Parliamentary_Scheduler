# Message Encoding and Decoding

This document explains the message encoding and decoding flow used in the repository.

## Files
- `include/msg/header.h`
- `include/msg/msg.h`
- `src/msg/header.c`
- `src/msg/msg.c`
- `test/msg/test_msg.c`

## Message Format
The message structure has three parts:
1. Header
2. Body
3. Payload buffer

### Header layout
Defined in `include/msg/header.h`:
- `Requester` (1 byte)
- `PriorityRequested` (1 byte)
- `SeqNum` (1 byte)
- `Localref` (1 byte)
- `BodyLen` (1 byte)

`FIXED_HDR_SIZE` is `5` bytes.

## Encoding
Implemented by `encode_msg()` in `src/msg/msg.c`.
- Computes total size as `FIXED_HDR_SIZE + FIXED_BDY_SIZE + BodyLen`.
- Writes header fields directly into the output buffer.
- Writes body type and return type after the header.
- Copies payload bytes from `msg->msg_buff` when present.
- Returns `1` on success and `0` on failure.

## Decoding
Implemented by `decode_msg()` in `src/msg/msg.c`.
- Calls `decode_header()` to parse the first `5` bytes.
- Calls `decode_body()` to parse body fields immediately after the header.
- Allocates `msg->msg_buff` for payload when `BodyLen > 0`.
- Copies payload bytes into the newly allocated buffer.
- Returns `1` on success and `0` on failure.

## Header Utilities
Implemented by `src/msg/header.c`.
- `decode_header()` reads bytes and populates `msgHeader`.
- `encode_header()` writes `msgHeader` fields into a raw buffer.

## Memory Management
Implemented by `free_msg()` in `src/msg/msg.c`.
- Frees the payload buffer allocated during decode.
- Sets `msg->msg_buff` to `NULL`.

## Tests
- `test/msg/test_msg.c` verifies:
  - `encode_msg()` produces the expected raw bytes.
  - `decode_msg()` reconstructs the original header and payload.
  - the returned success codes are correct.
