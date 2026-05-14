# Server Hierarchy

This document explains the server architecture and type hierarchy in the Parliamentary Scheduler repository.

## Overview
The server layer is currently defined across:
- `include/server/server.h` — server types and public API declarations
- `src/server/socket.c` — the current listening socket implementation

The hierarchy is built from low-level role definitions to runtime server context.

## Role and Metadata
### `srv_role`
Defines the server role.
- `srv_PRIME`
- `srv_SEC`
- `srv_MEMBER`
- `srv_OBSERVER`

### `srvMeta`
Encapsulates server metadata used for role advertisement and communication.
- `srv_role ServerRole`
- `uint8_t ServerCommAdv`

This is the lowest layer of the server hierarchy and is embedded in higher-level structures.

## Socket Configuration
### `srvSock`
Defines connection-level socket configuration.
- `srvMeta ServerMetaTag`
- `uint16_t ServerPort`

A `srvSock` describes the endpoint configuration for a server instance.

## Runtime Server Context
### `srvContext`
Represents a live server instance.
- `srvMeta meta`
- `uint16_t port`
- `int listen_fd`
- `bool running`

This is the top-level runtime structure used to track the active server state.

## Public Server API
`include/server/server.h` declares the main server operations:
- `int srv_StartListen(void);`
  - Starts the listening socket.
- `int srv_StopListen(void);`
  - Stops the active listener.
- `int srv_InitServer(srvContext *ctx, const srvSock *config);`
  - Initializes a server context from configuration data.
- `int srv_AcceptConnection(int listen_fd);`
  - Accepts a new connection from the listening socket.

## Current Implementation Status
- `src/server/socket.c` provides a stubbed `srv_StartListen()` implementation, including socket creation.
- Other declared APIs are not yet implemented in the source tree.

## Summary
The server hierarchy in this repository is:
1. `srv_role` — role identity
2. `srvMeta` — role metadata
3. `srvSock` — socket configuration
4. `srvContext` — runtime state
5. public APIs — lifecycle and connection operations

This hierarchy is meant to separate server identity/configuration from runtime socket state and operations.
