#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsgRec;
struct can_frame canMsgSend;

MCP2515 mcp2515(10);


void setup() {
  canMsgSend.can_id  = 0x18FF50E5;
  canMsgSend.can_dlc = 8;
  canMsgSend.data[0] = 0x0C;
  canMsgSend.data[1] = 0x80; // 320 V fake data
  canMsgSend.data[2] = 0x00;
  canMsgSend.data[3] = 0x5A; // 9A fake data
  canMsgSend.data[4] = 0b00010; // Send this status bit to be read from LSB order
  canMsgSend.data[5] = 0x00;
  canMsgSend.data[6] = 0x00;
  canMsgSend.data[7] = 0x00;
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- OBC ----------");
  // Serial.println("ID  DLC   DATA");
}

unsigned long last_time = 0;
void loop() {
  if (mcp2515.readMessage(&canMsgRec) == MCP2515::ERROR_OK) {
      Serial.print("ID: "); Serial.println(canMsgRec.can_id, HEX); 
      Serial.print("DLC: ");Serial.println(canMsgRec.can_dlc, HEX);
      Serial.print("Data(Bytes): ");
      for(short i = 0; i < canMsgRec.can_dlc; i++) {
        
        Serial.print(canMsgRec.data[i],HEX); Serial.print(" ");
      } Serial.println();    
    }
  // Detect CAN message from on board Charger ONLY -- Other CAN
  // Polling the Message From on board charger (Every 500 ms)
  if(millis()-last_time == 500) {
    

    // Condition to see if charger stop charging 
    // Read the control bit , display whether BMS command is to charge or stop the charge
    // Display Max V and A from it

    mcp2515.sendMessage(&canMsgSend);
    Serial.println("Messages sent");
    last_time = millis();
  }
  
}
