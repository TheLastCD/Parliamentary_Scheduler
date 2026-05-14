#include <stdint.h>
#include <stddef.h>
#include <server/server.h>
#include <msg/msg.h>

#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

/// Client role enumeration.
typedef enum {
  clt_PRIME,
  clt_SEC,
  clt_MEMBER,
  clt_OBSERVER,
} clt_role;

/// Client metadata, including the server metadata that the client reports to.
typedef struct {
  clt_role cltRole;
  srvMeta serverMeta; /**< Metadata for the server this client is associated with. */
} cltMeta;

int start_client(const cltMeta *client_meta, uint16_t client_id);
int kill_client(void);

int receive_msg(const uint8_t *packet, size_t len);
const srvMeta *route_msg(const Msg *msg);
int add_to_queue(const Msg *msg);

bool client_ping_response_sent(void);

#endif
