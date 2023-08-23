/****************************************************************
// The sample program of measurement with 1005500
// Author:T.Goto DGPS



//Rev.
//2023/3/30 drafting

************************************************************/
#ifndef INCLUDED_stdint_h_
#define INCLUDED_stdint_h_
#include<stdint.h>
#endif


//230711#define	ECHO_CANCEL

#define	COM_EXIST	(uint8_t)0U
#define	COM_CHECK	(uint8_t)1U
#define	COM_MEAS	(uint8_t)2U
#define	COM_STATUS	(uint8_t)3U
#define	COM_RF		(uint8_t)4U
#define	COM_VF		(uint8_t)5U

#define	RES_OK				(uint8_t)0U
#define	RES_GROUND_FAULT	(uint8_t)1U
#define	RES_ABNORMAL		(uint8_t)2U
#define	RES_BUSY			(uint8_t)4U

//for arduino I/O interface
//If you choose another platform, please modify this.
#define	DIGITAL_WRITE(...)	{ digitalWrite(__VA_ARGS__); }
#define	DIGITAL_READ	digitalRead
#define DELAY	delay //mS delay
#define	SERIAL_AVAILABLE	Serial.available
#define SERIAL_TX_BUF_EMPTY	Serial.flush	//waiting serial TX buffer empty

//for debug
#ifndef DBG
//#define _DEBUG 1//If debug, remove comment sign "//"
#if _DEBUG
#define DBG(...) { Soft_serial_1.print(__VA_ARGS__); }
#define DBGLED(...) { digitalWrite(__VA_ARGS__); }
#else
#define DBG(...)
#define DBGLED(...)
#endif /* _DBG */
#endif /* DBG */


/////////////////////
// function prototype
/////////////////////
int receive_response(uint8_t, uint8_t *, uint8_t *, uint8_t *);
int send_command(uint8_t, uint8_t *, uint8_t, uint8_t *);
int execute_command(uint8_t, uint8_t *);
int main2(long *, long *);
