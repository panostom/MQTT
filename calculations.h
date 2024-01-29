#ifndef __CALCULATIONS__
#define __CALCULATIONS__

#define INT_NUM_BYTES 4

byte Compute_CRC8(byte *array,unsigned int num_bytes)
{
  byte crc = 0;
  const byte generator=5;

  for(int i = 0; i<num_bytes; i++) //o int einai 4 bytes, ara 3ekinaw apo to aristero byte kai paw dexia gia na kanw crc
  {
    crc ^= array[i];
    for(int j = 0; j < 8 ; j++)
    {
      if((crc & 0x80) != 0)
      {
        crc = (byte)((crc << 1) ^ generator);
      }
      else
      {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void Compute_CRC8_bounds(char *crcDataMessage,unsigned int boundDelay,unsigned int boundTemperature, unsigned int boundHumidity) { //upologizw crc kathe oriou kai ftiaxnw to mhnyma
  byte dest[INT_NUM_BYTES];       //afou einai int ta oria, tote exw 4 bytes
  byte crc1, crc2, crc3;
  unsigned int crc1_t, crc2_t, crc3_t;

  for(int i=0;i<INT_NUM_BYTES;i++){
    dest[i]= (boundDelay >> 8*(INT_NUM_BYTES-1-i)) & 255;
  }
  Serial.println("4=" + String(dest[3]) + "  3=" + String(dest[2]) + "  2=" + String(dest[1]) + "  1=" + String(dest[0]));
  crc1 = Compute_CRC8(dest,INT_NUM_BYTES);
  Serial.println("crc1= " + String(crc1));

  for(int i=0;i<INT_NUM_BYTES;i++){
    dest[i]= (boundTemperature >> 8*(INT_NUM_BYTES-1-i)) & 255;
  }
  Serial.println("4=" + String(dest[3]) + "  3=" + String(dest[2]) + "  2=" + String(dest[1]) + "  1=" + String(dest[0]));
  crc2 = Compute_CRC8(dest,INT_NUM_BYTES);
  Serial.println("crc2= " + String(crc2));

  for(int i=0;i<INT_NUM_BYTES;i++){
    dest[i]= (boundHumidity >> 8*(INT_NUM_BYTES-1-i)) & 255;
  }
  Serial.println("4=" + String(dest[3]) + "  3=" + String(dest[2]) + "  2=" + String(dest[1]) + "  1=" + String(dest[0]));
  crc3 = Compute_CRC8(dest,INT_NUM_BYTES);
  Serial.println("crc3= " + String(crc3));

  crc1_t = (unsigned int) crc1;
  crc2_t = (unsigned int) crc2;
  crc3_t = (unsigned int) crc3;               //thelw na parw 3 digits arithmo
  //ekatontades      dekades     monades
  sprintf(crcDataMessage, "%u%u%u%u%u%u%u%u%u", crc1_t / 100, (crc1_t % 100) / 10, (crc1_t % 100) % 10, crc2_t / 100, (crc2_t % 100) / 10, (crc2_t % 100) % 10, crc3_t / 100, (crc3_t % 100) / 10, (crc3_t % 100) % 10);
}



#endif
