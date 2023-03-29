int receive_response(uint8_t *res_num, uint8_t *id, uint8_t *len, uint8_t *res){
//receive data structure
//data example    :  FE FE 68 00 00 00 00 00 00 68 81 08 65 F3 33 33 33 33 33 33 E3 16
//i               :   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21
//id_raw          :           00 00 00 00 00 00
//res             :                                      65 F3 33 33 33 33 33 33 E3 16
//j               :   0  0  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2
//k               :   0  0  0  0  0  0  0  0  0  0  0  0  0  1  2  3  4  5  6  7  8  9
//*len            :                                    8  8  8  8  8  8  8  8  8  8  8
  //variable
  uint32_t timer;
  uint8_t i,j,k,l,cs;
  uint8_t id_raw[6];
  uint8_t packet_ok, packet_end;
  uint8_t rx_data;

  //reset clock
  timer = millis();

  //receive
  i=0;j=0;k=0;l=0;cs=0;packet_ok=false;packet_end=false;*len=0;
  while(packet_end == false){
//@001    if(Serial.available() > 0){
    if(Serial2.available() > 0){//@001
//@001      rx_data = Serial.read();
      rx_data = Serial2.read();//@001
      if((i>=j+1) && (i<=j+6)){
        id_raw[i-(j+1)] = rx_data;
      }
//@002      if(i>j+9){
//@002      if(i>j+9 && k<sizeof(res)){//@002
      if(i>j+9 && k<10){//@002
        res[k++] = rx_data;
      }
      if(j==0 && rx_data == 0x68){//find first header
        j=i;
      }
      if(i==j+9){//length
        *len=rx_data;
      }
      if(i==j+9+*len+1){
        if(cs == rx_data){//cs=ok!
          packet_ok = true;
          for(l=0;l<6;l++){
            id[l] = id_raw[l];
          }
        }
      }
      if(i==j+9+*len+2){
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
    if(millis() - 500U > timer){//violation DLT645
      return false;
    }
  }//while(packet_end == false)

  //check packet
  if(packet_ok == false){//cs = NG!
    return false;
  }

  return true;//receive is OK!
}