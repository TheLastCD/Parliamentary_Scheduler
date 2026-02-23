#include <body/body.h>
#include <stdint.h>




uint8_t bdy_parse(msgBody *bdy){

	uint8_t type = bdy_type_parse(&bdy->BodyType);

}

uint8_t bdy_type_parse(bdy_type *type){
	return parse_byte;
}
uint8_t bdy_ret_parse(bdr_ret type);
uint8_t bdy_buff_parse(uint8_t *MsgBuffer);
