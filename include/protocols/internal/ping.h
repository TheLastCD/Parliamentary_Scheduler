#include <stdint.h>


typedef enum{
	INT_PING,
}ping_types;



typedef struct{
	uint8_t echo_data;
	uint8_t	stop_byte;
}send_ping_int;




