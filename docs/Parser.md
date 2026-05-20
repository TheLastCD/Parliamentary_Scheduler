# Central Parser for Incoming Requests

The central parser for incoming messages in this project is the `decode_msg()` function implemented in `src/msg/msg.c` and declared in `include/msg/msg.h`.

Key functions and files:

- `decode_msg()` — `src/msg/msg.c`
  - Top-level function that accepts a byte buffer and length, decodes the header and body, and extracts the payload into a newly allocated `Msg.msg_buff` when present.
  - It calls `decode_header()` and `decode_body()` and validates buffer lengths.
  - Usage example:
    ```c
    Msg msg = {0};
    if (decode_msg(&msg, buffer, len) > 0) {
        // process msg.hdr, msg.bdy, msg.msg_buff
        free_msg(&msg);
    }
    ```

- `decode_header()` — `src/msg/header.c`, declared in `include/msg/header.h`
  - Parses the fixed 5-byte header (Requester, PriorityRequested, SeqNum, Localref, BodyLen) and returns the header length (5).

- `decode_body()` — `src/msg/body.c`, declared in `include/msg/body.h`
  - Parses the 2-byte body header (BodyType, ReturnType) and returns its length (2).

Notes and integration points:

- Protocol handlers (e.g., `protocols/internal/*`) and higher-level modules (client/server) should call `decode_msg()` for any incoming wire data before processing.
- The callback manager expects a decoded `Msg` to be passed to `cb_trigger()`.
- The tape scheduler and routing logic examine `msg.hdr.PriorityRequested`, `msg.hdr.SeqNum`, and `msg.bdy.BodyType` to make queuing and routing decisions.

Where to extend:

- If you add new body types or change payload formats, update `decode_body()` and any associated payload parsing logic in `src/msg/payload.c`.
- For binary compatibility, keep `FIXED_HDR_SIZE` and `FIXED_BDY_SIZE` stable unless all components are updated together.

See also:
- [include/msg/msg.h](include/msg/msg.h)
- [src/msg/msg.c](src/msg/msg.c)
- [include/msg/header.h](include/msg/header.h)
- [include/msg/body.h](include/msg/body.h)
- [src/msg/payload.c](src/msg/payload.c)
