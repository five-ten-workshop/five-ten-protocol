int send_command(uint8_t com_num, uint8_t *id, uint8_t len, uint8_t *com){
  //cs
  int i,cs;
  
  //end of file check
  if(com_buff[com_num][0] == 0 && com_buff[com_num][1] == 0){
    return false;
  }

  //send a command
  //preunble
  cs=0;
  Serial.write(0xfe);
  Serial.write(0xfe);
  Serial.write(0xfe);
  //header
  Serial.write(0x68);cs+=0x68;
  //id
  for(i=0 ; i<6 ; i++){
    Serial.write(id[i]);cs+=id[i];   
  }
  //header
  Serial.write(0x68);cs+=0x68;
  //control code
  Serial.write(0x01);cs+=0x01;
  //length
  if(len > 2){
    Serial.write(len);cs+=len;
  }else{
    Serial.write(0x02);cs+=0x02;
  }
  //command
  Serial.write(com_buff[com_num][0]);cs+=com_buff[com_num][0];
  Serial.write(com_buff[com_num][1]);cs+=com_buff[com_num][1];
  if(len > 2){
    for(i=0 ; i<len ; i++){
      Serial.write(com[i]);cs+=com[i];
    }
  }  
  //check sum
  Serial.write(cs);
  //footer
  Serial.write(0x16);

  return true;
}
