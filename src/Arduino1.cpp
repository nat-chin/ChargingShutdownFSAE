/* Send And*/

#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsgRec;
struct can_frame canMsgSend;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  canMsgSend.can_id  = 0x1806E5F4;
  canMsgSend.can_dlc = 8;
  // Conserved Byte
  canMsgSend.data[5] = 0x00;
  canMsgSend.data[6] = 0x00;
  canMsgSend.data[7] = 0x00;
  
  mcp2515.reset(); // Reset SPI 
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
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
      // Detect CAN message from on board Charger ONLY -- Other CAN

      if(canMsgRec.can_id == 0x18FF50E5){
        // Check The Data Through Serial Monitor
        Serial.print(canMsgRec.can_id, HEX); Serial.print(" "); 
        Serial.print(canMsgRec.can_dlc, HEX); Serial.print(" ");
        for (int i = 0; i<canMsgRec.can_dlc; i++)  {  // print the data
          Serial.print(canMsgRec.data[i],HEX);
          Serial.print(" ");
        } Serial.println(); 
        // Monitor & Translate
        // Byte split from current Frame
        uint8_t VoutH = canMsgRec.data[0];
        uint8_t VoutL = canMsgRec.data[1];
        uint8_t AoutH = canMsgRec.data[2];
        uint8_t AoutL = canMsgRec.data[3];
        uint8_t stat =  canMsgRec.data[4];
        uint8_t swverH = canMsgRec.data[5];
        uint8_t swverL = canMsgRec.data[6];
        uint8_t hwver = canMsgRec.data[7];
        // Status byte translator
        

        // How to read status byte

      }
      // Serial.print(canMsgRec.can_id, HEX); Serial.print(" "); 
      // Serial.print(canMsgRec.can_dlc, HEX); Serial.print(" ");
      // for (int i = 0; i<canMsgRec.can_dlc; i++)  {  // print the data
      //   Serial.print(canMsgRec.data[i],HEX);
      //   Serial.print(" ");
      // } Serial.println();      

      
    }
  }

  // Initial CAN frame At Start Charge Time
  canMsgSend.data[0] = 0x03; // V highbyte 
  canMsgSend.data[1] = 0x3E; // V lowbyte
  canMsgSend.data[2] = 0x0E; // A Highbyte
  canMsgSend.data[3] = 0x0F; // A Lowbyte
  canMsgSend.data[4] = 0x00; // Control Byte

  if(millis()-last_time == 500) {
    mcp2515.sendMessage(&canMsgSend);
    // Serial.println("Messages sent");
    last_time = millis();
  }

}
