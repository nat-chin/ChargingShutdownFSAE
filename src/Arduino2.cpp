#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

can_frame obcsent;
can_frame bmsreceived;

MCP2515 mcp2515(10);


void setup() {
  obcsent.can_id  = 0x18FF50E5;
  obcsent.can_dlc = 8;
  obcsent.data[0] = 0x03;
  obcsent.data[1] = 0x20; // 800 * 0.1V/s = 80 V fake data
  obcsent.data[2] = 0x00;
  obcsent.data[3] = 0x32; // 5A fake data
  obcsent.data[4] = 0b00010; // Send this status bit to be read from LSB order
  obcsent.data[5] = 0x00;
  obcsent.data[6] = 0x00;
  obcsent.data[7] = 0x00;
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- OBC ----------");
  // Serial.println("ID  DLC   DATA");
}

unsigned long last_time = 0;
void loop() {
  if (mcp2515.readMessage(&bmsreceived) == MCP2515::ERROR_OK) {
      Serial.print("ID: "); Serial.println(bmsreceived.can_id, HEX); 
      Serial.print("DLC: ");Serial.println(bmsreceived.can_dlc, HEX);
      Serial.print("Data(Bytes): ");
      for(short i = 0; i < bmsreceived.can_dlc; i++) {
        
        Serial.print(bmsreceived.data[i],HEX); Serial.print(" ");
      } Serial.println();    
    }
  // Detect CAN message from on board Charger ONLY -- Other CAN
  // Polling the Message From on board charger (Every 500 ms)
  if(millis()-last_time == 500) {
    

    // Condition to see if charger stop charging 
    // Read the control bit , display whether BMS command is to charge or stop the charge
    // Display Max V and A from it

    mcp2515.sendMessage(&obcsent);
    Serial.println("Messages sent");
    last_time = millis();
  }
  
}
