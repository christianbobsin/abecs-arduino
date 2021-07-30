int incomingByte = 0;
unsigned char buffer[256];
int rec = 0;

#define ACK 0x06
#define SYN 0x16
#define NAK 0x15
#define ETB 0x17

#define END_BYTE rec-3

#define CRC_MASK 0x1021 /* x^16 + x^12 + x^5 + x^0 */


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.setTimeout(2000);
}


bool hasSYN()
{
  bool result = false;
  
  if (buffer[0] == SYN){

    result = true;
  }
  
  return result;
}

bool hasETB(){
  bool result = false;

  if ( buffer[END_BYTE] == ETB ){
    
    result = true;
  }  

  return result;
}

unsigned int CRC_Calc(unsigned char* pbData, int iLength) {
  
  unsigned int wData, wCRC = 0;
  int i;

  for (; iLength > 0; iLength--, pbData++)
  {
    wData = (unsigned int)(((unsigned int)*pbData) << 8);

    for (i = 0; i < 8; i++, wData <<= 1) 
    {
      if ((wCRC ^ wData) & 0x8000)
      {
        wCRC = (unsigned int)((wCRC << 1) ^ CRC_MASK);
      }
      else
      {
        wCRC <<= 1;
      }
    }
  }

  return wCRC;
}

bool evaluateCRC(unsigned char* pbData, int iLength) {

  unsigned int crc = CRC_Calc( pbData, iLength);

  bool crc_hi = pbData[iLength] & (crc >> 8);
  bool crc_lo = pbData[iLength +1] & (crc &0xFF);

  //Serial.write(0xFF);
  //Serial.write(pbData[iLength]);
  //Serial.write(pbData[iLength+1]);    
  //Serial.write(0xFF);

  return crc_hi & crc_lo;
}


void loop() {

  rec = Serial.available();
  
  
  if (rec > 0) {
    
    Serial.readBytes(buffer, rec);

    unsigned char* pbData = &buffer[1]; 

    if (hasSYN() && hasETB() && evaluateCRC( pbData, rec-3 )){

      Serial.write(ACK);

      //unsigned char* B = &buffer[1];       
      
      //unsigned int crc = CRC_Calc( B, rec-3);      
      //Serial.write(crc >> 8);
      //Serial.write(crc &0xFF); 
      
      //Serial.write(B[2]);
      
      Serial.write(pbData,rec-1);
    }
    else {

      Serial.write(NAK);
      
    }
    
    rec = 0;  
  }
  
  delay(500);
}
