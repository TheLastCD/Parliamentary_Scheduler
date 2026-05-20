# Message Types

This document describes the `Msg` layout, header and body enums, return types, payload semantics, and how the scheduler/callback systems use message fields.

## Overview
A message (`Msg`) is composed of:
- `msgHeader hdr` — fixed header metadata
- `msgBody bdy` — body metadata (type + return semantics)
- `MsgBuffer msg_buff` — optional payload bytes

See the implementation: [include/msg/msg.h](include/msg/msg.h)

## Encoding Layout
Serialized packet layout (encoded by `encode_msg`):
- 5-byte header (Requester, PriorityRequested, SeqNum, Localref, BodyLen)
- 2-byte body header (BodyType, ReturnType)
- `BodyLen` bytes of payload

Header encoding helpers: [include/msg/header.h](include/msg/header.h)
Body encoding helpers: [include/msg/body.h](include/msg/body.h)

## Header Fields
- `Requester` (uint8_t): ID of the message originator.
- `PriorityRequested` (`hdr_reqpriority`): scheduling priority requested for this message. Values:
  - `hdr_RED` — reserved/lowest
  - `hdr_LOW`
  - `hdr_MED`
  - `hdr_HIGH`
  - `hdr_INTR` — interrupt/highest
- `SeqNum` (uint8_t): local sequence number used for ordering/matching responses.
- `Localref` (uint8_t): group/local reference ID (used to determine routing to local client).
- `BodyLen` (uint8_t): length of the payload following header+body fields.

## Body Types (`bdy_type`)
Defined in [include/msg/body.h](include/msg/body.h). Current types:

- `bdy_NONE`
  - No operation / placeholder.
  - Typical ReturnType: `bdy_IGNORE`.

- `bdy_READ`
  - Request to read data or state. Payload encodes read parameters.
  - Expected ReturnType: `bdy_CONFIRM` or `bdy_MSG` (with read data in payload).
  - Handlers: `create_read()` / `create_read_response()` in `protocols/internal/read.c`.

- `bdy_WRITE`
  - Request to write or mutate state. Payload carries write data.
  - Expected ReturnType: `bdy_CONFIRM` or `bdy_MSG` for follow-up messages.
  - Handlers: `create_write()` / `create_write_response()` in `protocols/internal/write.c`.

- `bdy_COPY`
  - Request to copy or replicate data.
  - Expected ReturnType: `bdy_CONFIRM`.
  - Handlers: `create_copy()` / `create_copy_response()` in `protocols/internal/copy.c`.

- `bdy_PING`
  - Heartbeat / liveness check. Payload is optional echo data.
  - Expected ReturnType: `bdy_CONFIRM` to acknowledge or `bdy_MSG` for extended reply.
  - Handlers: `create_ping()` / `create_ping_response()` in `protocols/internal/ping.c`.

## Return Types (`bdr_ret`)
Defined in [include/msg/body.h](include/msg/body.h):
- `bdy_IGNORE` — no reply required.
- `bdy_CONFIRM` — simple confirm/acknowledgement expected.
- `bdy_MSG` — a full message response (likely with payload) is expected.

## Payload Semantics
- Payload is a raw byte buffer (`MsgBuffer`) whose interpretation depends on `BodyType`.
- `payload_parse()` and `payload_dir()` exist to validate/route payload handling: [src/msg/payload.c](src/msg/payload.c)

## Scheduler Interaction
- `PriorityRequested` is used by the tape scheduler (`include/scheduler/tape.h`) when ordering queued messages.
- The tape enqueue routine prefers higher `PriorityRequested` values and breaks ties using `SeqNum`.
- See: `tape_enqueue_msg()` and `tape_take_head()` in `src/scheduler/tape.c`.

## Callback Integration
- The callback manager registers callbacks keyed by `message_type` (body type) and `response_type` (return type).
- To trigger a callback when a response arrives, register with `cb_register()` using a `callback_config_t`:

```c
callback_config_t cfg = {
  .message_type = bdy_PING,
  .response_type = bdy_CONFIRM,
  .on_response = my_ping_cb,
  .filter = NULL,
  .context = user_ctx,
};
int id = cb_register(&cfg);
```

- `cb_trigger()` is invoked with a decoded `Msg` to execute matching callbacks.
- See: `include/callbacks/callback_manager.h` and `src/callbacks/callback_manager.c`.

## Examples
- Example usage and tests:
  - Message encode/decode: [test/msg/test_msg.c](test/msg/test_msg.c)
  - Tape scheduler ordering: [test/scheduler/test_tape.c](test/scheduler/test_tape.c)
  - Ping protocol tests: [test/protocols/internal/test_ping.c](test/protocols/internal/test_ping.c)
  - Callback example: [examples/callback_example.c](examples/callback_example.c)

## Notes & Extending
- When adding new `bdy_type` values, add matching `protocols/internal/<type>.c` and header, plus tests under `test/protocols/internal/` and add the source to `CMakeLists.txt`.
- Keep `BodyLen` accurate when encoding: `encode_msg()` uses it to determine payload copy length.

---
Generated on May 19, 2026.
