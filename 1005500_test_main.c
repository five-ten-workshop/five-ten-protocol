/**********************************************
//This tests 1005500 via serial I/F.
//Copy rights : DG Capital Group
//Auth. T.Goto
//2023/4/18 Rev.0
**********************************************/
#ifndef _1005500_test_main_h
#define _1005500_test_main_h
#include "1005500_test_main.h"
#endif

//#define ECHO_CANCEL

/****************************************************************
// The sample program of measurement
// Author:T.Goto DGPS


// The flow chart of one cycle

//confirmation of existance
	//loop
		//sending broad cast
		//if receive the response and true, break
		//if 3 count, return false
		//wait
		//continue
	//return response

//run the self checking (prepare of measurement)
	//loop
		//send checking command
		//if receive the response and true, break
		//if 3 count, return false
		//wait
		//continue
	//return response

//run the measurement
	//loop
		//send measurement command
		//if receive the response and true, break
		//if 3 count, return false
		//wait
		//continue
	//return response

//pick up the measurement data
	//loop
		//send pick-up command of RF
		//if receive the response and true, break
		//if 3 count, return false
		//wait
		//continue
	//return response
	//loop
		//send pick-up command of RF
		//if receive the response and true, break
		//if 3 count, return false
		//wait
		//continue
	//return response

//return the result of measurement

//Rev.
//2023/3/30 drafting

************************************************************/
//implementation of command
const uint8_t com_buff[8][2] = {
	{0x32, 0xC0},//0
	{0x00, 0xF0},
	{0x01, 0xF0},
	{0x02, 0xF0},
	{0x03, 0xF0},
	{0x04, 0xF0},
	{0x05, 0xF0},
	{0x00, 0x00}//EOF
};


//uint8_t id[6];

int receive_response(uint8_t com_num, uint8_t *id, uint8_t *len, uint8_t *res_data){
	bool master;
	uint8_t com_recv[2];
	int i;

	while(1){
		if(receive_dlt645(com_recv, id, len, res_data, &master) == 1){
			return 1;
		}else if(master == true){//receive the packet only from master.
			return 1;//receiving is failed.//230711
		}else{//the packet were sent from slave.
			DBG("receive_response:rcv645():com_recv=");DBG((int)*com_recv, HEX);DBG(" len=");DBG((int)*len, HEX);DBG(" res_data=");DBG((int)res_data[0], HEX);DBG("\n");
			break;//receiving is succeeded.
		}
	}//while(1)
	//if you need matching of id, describe from here.

	//com_num matching
	if( *((uint16_t *)com_buff[com_num]) == *((uint16_t *)com_recv )) {
		return 0;//match the command
	}//if

	return 1;//abnormal end with other reason.
}


int send_command(uint8_t com_num, uint8_t *id, uint8_t len, uint8_t *com_data){
	uint8_t com_recv[2];
	bool master;
	uint8_t *res;
	uint8_t	rcv_len;
	uint8_t rcv_res[8];
	uint8_t rcv_id[6];

	DELAY(2);//wait 11bit(1.145mS) for txd flushing
	DIGITAL_WRITE(COM_DIRECTION, TX);
	if(send_dlt645(com_buff[com_num], id, len, com_data, true) == 1){//@@@//230822
		DBG("send_command:send_dlt645() failed.\n");
		return 1;
	}
	SERIAL_TX_BUF_EMPTY();//To wait TX buffer empty.
	DELAY(2);//wait 11bit(1.145mS) for txd flushing
	DIGITAL_WRITE(COM_DIRECTION, RX);
#ifdef ECHO_CANCEL
	if(SERIAL_AVAILABLE()!=0){
		DBG("send_response:Enter the echo cancelling\n");
		receive_dlt645(com_recv, rcv_id, &rcv_len, rcv_res, &master);
		DBG("send_response:Exit from the echo cancelling\n");
	}
#endif

	return 0;//command succeeded.
}



int execute_command(uint8_t com_num, uint8_t *result){
	int i=0;
	uint8_t len=0;
	uint8_t *com_data=NULL;
	static uint8_t id[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99};

	for(i=0;i<3;i++){
		if(send_command(com_num, id, len, com_data) == 1){
			DBG("main:execute_command:send_command() failed.\n");
			continue;
		}
		if(receive_response(com_num, id, &len, result) == 1){
			continue;
		}else{
			return 0;//command succeeded.
		}
	}
	return 1;//command failed.
}


int main2(long *measure_data_rf, long *measure_data_vf){
	
	uint8_t result[8];

	//I/O settings. Set the RS485 direction port to OUTPUT.
	pinMode(COM_DIRECTION, OUTPUT);

	//run the confirmation of existance
	DBG("main:Confirm of existance\n");
	if(execute_command(COM_EXIST, result) != 0){
		DBG("main:execute_command()1 failed.\n");
		return(1);// none of response
	}
	
	//run the self checking
	DBG("main:Start the self checking\n");
	if(execute_command(COM_CHECK, result) != 0){
		DBG("main:execute_command()2 failed.\n");
		return(1);// none of response
	}
	
	//run the measurement
	DBG("main:Start the measurement\n");
	if(execute_command(COM_MEAS, result) != 0){
		DBG("main:execute_command()3 failed.\n");
		return(1);// none of response
	}

	//check the status
	DBG("main:Check Status\n");
	while(1){
		if(execute_command(COM_STATUS, result) != 0){
			DBG("main:execute_command()4 failed.\n");
			return(1);// none of response
		}
		if(((*(uint16_t *)result) & 4U) == 4U){
			DBG(".");
			DELAY(1000);//wait 1 sec.
		}else{//230719
			break;//230719
		}
	}
	DBG("\n");
	if(((*(uint16_t *)result) & 2U) == 2U){
		DBG("main:Fault at self checking\n");
		return(1);// malfunction
	}
	if(((*(uint16_t *)result) & 1U) == 1U){
		DBG("main:Fault at ground fault\n");
//		return(1);// ground fault
	}

	
	//pick up the measurement data
	DBG("Pick up the measurement data of RF\n");
	if(execute_command(COM_RF, result) != 0){
		DBG("main:Fault at measuring RF\n");
		return(1);// none of response
	}
	DBG("The RF(ohm) is ");	DBG(*(uint32_t *)result);DBG("\n");
	*measure_data_rf = *(uint32_t *)result;
	
	//pick up the measurement data
	DBG("Pick up the measurement data of VF\n");
	if(execute_command(COM_VF, result) != 0){
		DBG("main:Fault at measuring RF\n");
		return(1);// none of response
	}
	DBG("The VF(V) is ");DBG(*(uint32_t*)result);DBG("\n");
	*measure_data_vf = *(uint32_t *)result;

	DBG("Successfully measured.\n");
	return(0);
}
