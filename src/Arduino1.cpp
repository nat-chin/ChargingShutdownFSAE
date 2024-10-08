/* Send And*/

#include <Arduino.h>
#include <ArduinoSTL.h> // many C/C++ standard libs comeback
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsgRec;
struct can_frame canMsgSend;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  /* Set up BMS CAN frame*/
    canMsgSend.can_id  = 0x1806E5F4;
    canMsgSend.can_dlc = 8;
    // Conserved Byte
    canMsgSend.data[5] = 0x00;
    canMsgSend.data[6] = 0x00;
    canMsgSend.data[7] = 0x00;
  
  /* MCP2515 setup */
  mcp2515.reset(); // Reset SPI 
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- BMS CAN Enabled ----------");
  Serial.println("ID  DLC   DATA");
}

// Split and merge High byte low byte of 16 bit unsigned integer
uint8_t *splitHLbyte(float num){
  static uint8_t temp[2] = {0,0}; // initialize
  
  return temp;
}
// HighByte = splitHLbyte[0]
// LowByte = splitHLbyte[1]
float mergeHLbyte(uint8_t Hbyte, uint8_t Lbyte){
  static uint16_t temp = 0;
  // bitshiftLeft , then put to different variable
  return temp;
}

// Encode float or uint16 into arrays of uint8
uint8_t *Encode_bytearray(float f) { 
    // Use memcpy to copy the bytes of the float into the array
    static uint8_t c[sizeof(f)]; 
    memcpy(c, &f, sizeof(f));
    // Copy to address of array , Copy from address of float , size of float
    // Now, c[0] to c[3] contain the bytes of the float
    return c; 
}

// This is for voltage monitoring

float Decode_bytearray(unsigned char* c) {
    float f;
    // Use memcpy to copy the bytes from the array back into the float
    memcpy(&f, c, sizeof(f));
    return f;
}

unsigned long last_time = 0;
void loop() {
  /*
  Minimum Voltage of 2 Module : 3.1*20 = 62V , Nominal Volage 3.6*20 = 72V
  Maximum allowable Voltage for 2 module : 83V => 830 => 0x 03 3E
  Maximum allowable current for 2 module : 5A => 50 => 0x 00 32 
  */
  
  // Polling the Message From on board charger (Every 500 ms)
  if(millis()-last_time == 500){
    if (mcp2515.readMessage(&canMsgRec) == MCP2515::ERROR_OK) {

      // This block of code execute if detect CAN message from OBC ONLY--
      if(canMsgRec.can_id == 0x18FF50E5){
        // Check The Data Through Serial Monitor
        Serial.print(canMsgRec.can_id, HEX); Serial.print(" "); 
        Serial.print(canMsgRec.can_dlc, HEX); Serial.print(" ");
        for (int i = 0; i<canMsgRec.can_dlc; i++)  {  // print the data
          Serial.print(canMsgRec.data[i],HEX);
          Serial.print(" ");
        } Serial.println(); 

        // Monitor & Translate current Frame data
        uint8_t VoutH = canMsgRec.data[0];
        uint8_t VoutL = canMsgRec.data[1];
        uint8_t AoutH = canMsgRec.data[2];
        uint8_t AoutL = canMsgRec.data[3];
        // Merge High byte with low byte
        float OBCVolt = mergeHLbyte(VoutH,VoutL);
        float OBCAmp = mergeHLbyte(AoutH,AoutL);

        // Interpret OBC status, and decide on Shutdown command
        uint8_t stat =  canMsgRec.data[4];
        // Status byte translator


        // How to read status byte
        // Detect Communication Error



        // uint8_t swverH = canMsgRec.data[5];
        // uint8_t swverL = canMsgRec.data[6];
        // uint8_t hwver = canMsgRec.data[7];
        

      }
    }
  }



  // Normal BMS message during charge
  canMsgSend.data[0] = 0x03; // V highbyte 
  canMsgSend.data[1] = 0x3E; // V lowbyte
  canMsgSend.data[2] = 0x00; // A Highbyte
  canMsgSend.data[3] = 0x32; // A Lowbyte
  canMsgSend.data[4] = 0x00; // Control Byte

  // Message During Shutdown
  canMsgSend.data[0] = 0x00; // V highbyte 
  canMsgSend.data[1] = 0x00; // V lowbyte
  canMsgSend.data[2] = 0x00; // A Highbyte
  canMsgSend.data[3] = 0x00; // A Lowbyte
  canMsgSend.data[4] = 0x01; // Control Byte


  // Transmitting Control Message
  if(millis()-last_time == 500) {
    mcp2515.sendMessage(&canMsgSend);
    // Serial.println("Messages sent");
    last_time = millis();
  }

}
