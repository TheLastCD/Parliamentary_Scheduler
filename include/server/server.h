#include <stdint.h>



typedef enum{
  srv_PRIME, 
  srv_SEC,
  srv_MEMBER,
  srv_OBSVER,
}srv_role;



typedef struct{
	srv_role ServerRole; 
	uint8_t ServerCommAdv;
}srvMeta;




// comms type structs
typedef struct{
	srvMeta ServerMetaTag; 
	uint16_t ServerPort;
}srvSock;
