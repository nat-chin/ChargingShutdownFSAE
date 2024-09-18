#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsgRec;
struct can_frame canMsgSend;

MCP2515 mcp2515(10);


void setup() {
  canMsgSend.can_id  = 0x02;
  canMsgSend.can_dlc = 2;
  canMsgSend.data[0] = 0xAA;
  canMsgSend.data[1] = 0xBB;

  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

unsigned long last_time = 0;

void loop() {
  if (mcp2515.readMessage(&canMsgRec) == MCP2515::ERROR_OK) {
    Serial.print(canMsgRec.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsgRec.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsgRec.can_dlc; i++)  {  // print the data
      Serial.print(canMsgRec.data[i],HEX);
      Serial.print(" ");
    }
    Serial.println();      
  }
  if(millis()-last_time == 500) {
    mcp2515.sendMessage(&canMsgSend);
    // Serial.println("Messages sent");
  }
  
  
}
