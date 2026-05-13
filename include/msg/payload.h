#ifndef MSG_PAYLOAD_H
#define MSG_PAYLOAD_H

#include <stdint.h>
#include <msg/body.h>


typedef uint8_t *MsgBuffer; //payload

typedef enum {
	pyld_VALID,
	pyld_INVALID,
	pyld_ERROR,
}pyld_validity;



pyld_validity payload_ctl(MsgBuffer payload);

int payload_dir(MsgBuffer payload, bdy_type bdyType);
int payload_parse(MsgBuffer payload, bdy_type bdyType);

#endif // MSG_PAYLOAD_H

