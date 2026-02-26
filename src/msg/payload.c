#include <msg/payload.h>



// pyld_validity payload_ctl(MsgBuffer payload){
// 	return pyld_VALID;
// }
//
int payload_dir(MsgBuffer payload, bdy_type bdyType){
	switch(bdyType){
		case bdy_NONE: 
		case bdy_PING:
			return 0;
		case bdy_READ:
		case bdy_WRITE:
		case bdy_COPY:
			return -1;

		default:
			return 1;
	};
}

// int payload_parse(MsgBuffer payload, bdy_type bdyType);
