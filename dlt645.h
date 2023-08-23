// DL/T645 protocol communication library
// Copy right DGcapital group
// Author:T.Goto
// 2023/4/18 first edition
//
//
//


//for arduino libraries
#ifndef dlt645_h
#define dlt645_h

//
//define
//

//for arduino standard serial I/F
//If you choose another platform, please modify this.
#define	SERIAL_WRITE(...)	{ Serial.write(__VA_ARGS__); }
#define	SERIAL_READ	Serial.read
#define	SERIAL_AVAILABLE	Serial.available

//for use of uint_x macro
#ifndef	INCLUDED_stdint_h_
#define	INCLUDED_stdint_h_
#include<stdint.h>
#endif

//for timer function
//Custom those macro for your platform.
#define	CLOCK_T	uint32_t//for arduino
#define	CLOCK	millis//for arduino
#define	TIMEOUT	500U//500mS at arduino

//
//prototype definition
//

void encoder33(uint8_t *,uint8_t);
void decoder33(uint8_t *,uint8_t);
int receive_dlt645(uint8_t *, uint8_t *, uint8_t *, uint8_t *, bool *);
int send_dlt645(uint8_t *, uint8_t *, uint8_t, uint8_t *, bool);

#endif
