// DL/T645 protocol communication library
// Copy right DGcapital group
// Author:T.Goto
// 2023/4/18 first edition
// 2023/9/8 second edition
//
//
//
#include "dlt645.h"

/////////////////////////
// encode 0x33
/////////////////////////

void encoder33(uint8_t *in_data, uint8_t in_data_cnt){
	uint8_t i;
	for(i=0;i<in_data_cnt;i++){
		in_data[i]+=(uint8_t)0x33U;
	}
}


////////////////////////
// decode 0x33
/////////////////////////

void decoder33(uint8_t *in_data, uint8_t in_data_cnt){
	uint8_t i;
	for(i=0;i<in_data_cnt;i++){
		in_data[i]-=(uint8_t)0x33U;
	}
}


/////////////////////////
// receive_data
/////////////////////////

int receive_dlt645(uint8_t *command, uint8_t *id, uint8_t *len, uint8_t *data, bool *master){
//receive data structure
//data example    :  FE FE 68 00 00 00 00 00 00 68 81 08 65 F3 33 33 33 33 33 33 E3 16
//i               :   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21
//id_raw          :           00 00 00 00 00 00
//command         :                                      65 F3                        
//data            :                                            33 33 33 33 33 33 E3 16
//j               :   0  0  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2
//k               :   0  0  0  0  0  0  0  0  0  0  0  0  0  1  2  3  4  5  6  7  8  9
//*len(only data) :                                    6  6  6  6  6  6  6  6  6  6  6
	//variable
//	uint32_t timer;
	CLOCK_T timer;
	uint8_t i,j,k,l,cs;
	uint8_t id_raw[6];
	bool packet_ok, packet_end;
	uint8_t rx_data;
	bool found_header = 0;//230718

	//reset clock
	timer = CLOCK();

	//receive
	i=0;j=0;k=0;l=0;cs=0;packet_ok=false;packet_end=false;*len=0;
	while(packet_end == false){
		if(SERIAL_AVAILABLE() > 0){
			rx_data = SERIAL_READ();
			if((rx_data != 0xFE) && (!found_header)){
				continue;
			}
			found_header = true;
			if((i>=j+1) && (i<=j+6)){
				id_raw[i-(j+1)] = rx_data;
			}
			if(i==j+8){
				*master = ((rx_data & 0x80) == 0);
			}
			if(i>j+9 && k<2){
				command[k] = rx_data;
				decoder33(&(command[k]), 1);
				k++;
			}
			if(i>j+11 && k<10 && k>=2){
				data[k-2] = rx_data;
				decoder33(&(data[k-2]), 1);
				k++;
			}
			if(j==0 && (rx_data == 0x68)){//find first header
				j=i;
				timer = CLOCK();
			}
			if(i==j+9){//length
				*len=rx_data - 2;
				if(*len>8){
					*len = 8;
				}
			}
			if(i==j+9+*len+2+1){
				if(cs == rx_data){//cs=ok!
					packet_ok = true;
					for(l=0;l<6;l++){
						id[l] = id_raw[l];
					}
				}
			}
			if(i==j+9+*len+2+2){
				if(rx_data == 0x16){
					packet_end = true;
				}
			}
			if(j>0){//calculate cs
				cs+=rx_data;
			}
			i++;
		}//if(Serial.available() > 0)

		//check timer
		if(CLOCK() - timer > TIMEOUT){//violation DLT645
			return 1;
		}
	}//while(packet_end == false)

	//check packet
	if(packet_ok == false){//cs = NG!
		return 1;
	}

	return 0;//receive is OK!
}


/////////////////////////
// send_data
/////////////////////////

int send_dlt645(uint8_t *command, uint8_t *id, uint8_t len, uint8_t *data, bool master){
// command:		command to send
// id:				id of meter at BCD. broad cast is 0x999999999999. if less than 12 digits, paste with 0xaa. The order is little endian:[0],[1],[2]... 
// len:				length of data only
// data:			the data for tranfer	
// master:		if master = 1, This transfer direction is from master to slave. if 0, from slave to master.

//	DBG("dlt645:send_dlt645() start.\n");
	//cs
	int i,cs;
	uint8_t control_code=0;
	uint8_t command_cpy[2];

	//end of file check
	if(command[0] == 0 && command[1] == 0){
		return 1;
	}

	//send a command
	//preunble
	cs=0;
	SERIAL_WRITE(0xfe);
	SERIAL_WRITE(0xfe);
	SERIAL_WRITE(0xfe);
	//header
	SERIAL_WRITE(0x68);cs+=0x68;
	//id
	for(i=0 ; i<6 ; i++){
		SERIAL_WRITE(id[i]);cs+=id[i];	 
	}
	//header
	SERIAL_WRITE(0x68);cs+=0x68;
	//control code
	control_code |= 0x01;//function is read data.
	if(master==false){
		control_code |= 0x80;//this is response
	}
	SERIAL_WRITE(control_code);cs+=control_code;
	//length
	if(len > 0){//@@@
		SERIAL_WRITE(len + 2U);cs+=len + 2U;
	}else{
		SERIAL_WRITE(2U);cs+=2U;//@@@
	}
	//command
	command_cpy[0] = command[0];
	command_cpy[1] = command[1];
	encoder33(command_cpy, 2);
	SERIAL_WRITE(command_cpy[0]);cs+=command_cpy[0];
	SERIAL_WRITE(command_cpy[1]);cs+=command_cpy[1];
	//data
	if(len > 0){
		encoder33(data, len);
		for(i=0 ; i<len ; i++){
			SERIAL_WRITE(data[i]);cs+=data[i];
		}
	}	
	//check sum
	SERIAL_WRITE(cs);
	//footer
	SERIAL_WRITE(0x16);

	return 0;
}
