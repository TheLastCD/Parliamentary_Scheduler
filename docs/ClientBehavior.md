# Client Behavior

This document describes how the client component works in the Parliamentary Scheduler repository.

## Client Metadata
The client metadata is defined in `include/client/client.h`:
- `cltRole` — the role assigned to the client.
- `serverMeta` — metadata describing the server the client is associated with.

This makes it explicit that a client knows its own role and the server it reports to.

## Client Lifecycle
The client runtime is managed in `src/client/client.c`.

### start_client()
- Initializes client state.
- Stores the client role and server metadata.
- Marks the client as running.
- Sets the next server target from `client_meta.serverMeta`.

### kill_client()
- Stops the client.
- Frees all queued messages.
- Clears internal runtime state.

## Message Reception and Routing
The main client message flow is:
1. `receive_msg()` accepts a raw encoded packet.
2. It decodes the packet into a `Msg` structure.
3. `route_msg()` decides whether the packet is intended for this client.

### Local delivery
If the incoming message `Localref` equals the client ID:
- the client processes the packet locally via `process_packet()`.
- `route_msg()` returns `NULL` to indicate no server forwarding is needed.

### Forwarding to the next server
If the message is not addressed to this client:
- `route_msg()` returns the current `next_server` metadata.
- `receive_msg()` enqueues the decoded message by calling `add_to_queue()`.

### Queue behavior
- `add_to_queue()` clones the decoded message and stores it in the client queue.
- The queue is bounded by `CLIENT_QUEUE_MAX`.

## Server-to-client packet processing
A server-delivered packet is represented the same way as any received raw packet.
When `Localref == client_id`, the client processes it locally.

## Tests
The client behavior is covered by `test/client/test_client.c`:
- `test_client_routing()` verifies a non-local message is routed toward the next server.
- `test_client_process()` verifies a message addressed to the client is processed locally.
- `test_client_process_from_server()` verifies a server-originated packet intended for the client is accepted and processed.
