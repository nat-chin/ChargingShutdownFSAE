/* BMS Master unit Send And Receive to and from OBC*/

#include <Arduino.h>
#include <ArduinoSTL.h> // C/C++ standard libs comeback
#include <SPI.h>
#include <mcp2515.h>

// Byte conversion , bit shifting custom functions
#include <util.h>

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
        // Serially Monitor The Data
        Serial.print("ID: "); Serial.println(canMsgRec.can_id, HEX); 
        Serial.println();Serial.println(canMsgRec.can_dlc, HEX);
        for (int i = 0; i < canMsgRec.can_dlc; i++)  {  // print the data
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
        uint8_t status =  canMsgRec.data[4];
        unsigned char* statusBits = checkLSB(status);
        // It may be more efficient to instead of returning an array check each one if eq. to 0 or 1
        // usually bit 1 in any position indicates somekind of faults
        // Status byte translator
        switch (expression)
        {
        case /* constant-expression */:
          /* code */
          break;
        
        default:
          break;
        }


        // How to read status byte
        // Detect Communication Error



        // uint8_t swverH = canMsgRec.data[5];
        // uint8_t swverL = canMsgRec.data[6];
        // uint8_t hwver = canMsgRec.data[7];
        

      }
    }
  }



  // Condition 1 Normal BMS message during charge
  canMsgSend.data[0] = 0x03; // V highbyte 
  canMsgSend.data[1] = 0x3E; // V lowbyte
  canMsgSend.data[2] = 0x00; // A Highbyte
  canMsgSend.data[3] = 0x32; // A Lowbyte
  canMsgSend.data[4] = 0x00; // Control Byte 0 charger operate

  // Condition 2 Message During Shutdown
  canMsgSend.data[0] = 0x00; // V highbyte 
  canMsgSend.data[1] = 0x00; // V lowbyte
  canMsgSend.data[2] = 0x00; // A Highbyte
  canMsgSend.data[3] = 0x00; // A Lowbyte
  canMsgSend.data[4] = 0x01; // Control Byte 1 charger shutdown
  // Turn maximum allowable voltage and current to 0 also as fail safe

  // Send TTL LOW to Charging shutdown (Passed Through opto)


  // Transmitting Control Message
  if(millis()-last_time == 500) {
    mcp2515.sendMessage(&canMsgSend);
    // Serial.println("Messages sent");
    last_time = millis();
  }

}
