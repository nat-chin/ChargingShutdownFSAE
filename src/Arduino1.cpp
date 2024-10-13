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
#define SDCPIN 4

void setup() {
  digitalWrite(SDCPIN,HIGH);
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
  
  /* BMS Native Feature*/
    float cellvolt = 5;
    float dischgAmp = 5;
    // Function to read individual Cell voltage once
    Serial.println();
    Serial.print("Cell Voltage: "); Serial.println(cellvolt,DEC);

  /* Minimum Voltage of 2 Module : 3.1*20 = 62V , Nominal Volage 3.6*20 = 72V
  Maximum allowable Voltage for 2 module : 83V => 830 => 0x 03 3E
  Maximum allowable current for 2 module : 5A => 50 => 0x 00 32 */
  // Polling the Message From on board charger (Every 500 ms)
  /* Use PCANView to monitor the Bus instead */
    // Serial.print("ID: "); Serial.println(canMsgRec.can_id, HEX); 
    // Serial.print("DLC: ");Serial.println(canMsgRec.can_dlc, HEX);
    // Serial.print("Data(Bytes): ");
    // for(int i = 0; i < canMsgRec.can_dlc; i++) {
    //   Serial.print(canMsgRec.data[i],HEX); Serial.print(" ");
    // } Serial.println(); 
  status STAT;
  // float OBCVolt = 0;
  // float OBCAmp = 0;

  // Transmitting Control Message (Every 500ms as well)
  if(millis()-last_time == 500) {
    
    
    // This can be problematic as there needs to be a first message to make the OBC not entering COMMUNICATION ERROR
    // Should I condition this as well , or just leave it be??
    // Condition 1 Normal BMS message during charge
    canMsgSend.data[0] = 0x03; // V highbyte 
    canMsgSend.data[1] = 0x3E; // V lowbyte
    canMsgSend.data[2] = 0x00; // A Highbyte
    canMsgSend.data[3] = 0x32; // A Lowbyte
    canMsgSend.data[4] = 0x00; // Control Byte 0 charger operate
    // Not sure if there should be any change to V,A cap limit , just fixed the number by now
    // Send TTL LOW to Charging shutdown (Passed Through opto) [/]
    mcp2515.sendMessage(&canMsgSend);
    // Serial.println("Messages sent");

    // Condition 2 Message During Shutdown
    if(STAT.shutdownsig == 0){
      canMsgSend.data[0] = 0x00; // V highbyte 
      canMsgSend.data[1] = 0x00; // V lowbyte
      canMsgSend.data[2] = 0x00; // A Highbyte
      canMsgSend.data[3] = 0x00; // A Lowbyte
      canMsgSend.data[4] = 0x01; // Control Byte 1 charger shutdown
      // Turn maximum allowable voltage and current to 0 also as fail safe
      mcp2515.sendMessage(&canMsgSend);
    }
  
    if (mcp2515.readMessage(&canMsgRec) == MCP2515::ERROR_OK) {

      // This block of code execute if detect CAN message from OBC ONLY--
      if(canMsgRec.can_id == 0x18FF50E5){
        // Monitor & Translate current Frame data
        uint8_t VoutH = canMsgRec.data[0];
        uint8_t VoutL = canMsgRec.data[1];
        uint8_t AoutH = canMsgRec.data[2];
        uint8_t AoutL = canMsgRec.data[3];
        // Merge High byte with low byte
        float OBCVolt = mergeHLbyte(VoutH,VoutL);
        float OBCAmp = mergeHLbyte(AoutH,AoutL);
        // OBCVolt = mergeHLbyte(VoutH,VoutL);
        // OBCAmp = mergeHLbyte(AoutH,AoutL);
        Serial.print("Charging Voltage[V]: "); Serial.println(OBCVolt,DEC);
        Serial.print("Charging Current[A]: "); Serial.println(OBCAmp,DEC);
        
        /* Interpret OBC status, and decide on Shutdown command */
        uint8_t stat =  canMsgRec.data[4]; // Status Byte
        checkstatLSB(&STAT,stat);

        // Shutdown if found any status bit as 1 (abnormal scenario)
        if(STAT.shutdownsig){
          digitalWrite(SDCPIN,HIGH);
          Serial.print("SHUTDOWN: OK ");
        } else {
          digitalWrite(SDCPIN,LOW);
          Serial.print("SHUTDOWN: NOT_OK ");
          // Shutdown , send the command to OBC to stop , then stop CAN
        }
        // Intepret Individual bit meaning
        Serial.print("OBC status: "); 
        for (short i =0 ; i <8 ; i++)
          Serial.print(STAT.statbin[i],DEC);
        Serial.println();
        
        switch (STAT.statbin[0]) {
          case 0:
            Serial.println("ChargerHW = Normal");
            break;
          
          case 1:
            Serial.println("ChargerHW = Faulty");
            break;
        }
        switch (STAT.statbin[1]) {
          case 0:
            Serial.println("ChargerTemp = Normal");
            break;
          
          case 1:
            Serial.println("ChargerTemp = Overheat");
            break;
        }
        switch (STAT.statbin[2]) {
          case 0:
            Serial.println("ChargerACplug = Normal");
            break;
          
          case 1:
            Serial.println("ChargerACplug = Reversed");
            break;
        }
        switch (STAT.statbin[3]) {
          case 0:
            Serial.println("Charger detects: Vbatt");
            break;
          
          case 1:
            Serial.println("Charger detects: ZERO Vbatt");
            break;
        }
        switch (STAT.statbin[4]) {
          case 0:
            Serial.println("COMMUNICATION STATUS: Normal");
            break;
          
          case 1:
            Serial.println("COMMUNICATION STATUS: Time out (6s)");
            break;
        }

        // How to read status byte
        // Detect Communication Error
        // communication timeout on the side of Charger (No command message from BMS is already dealt within OBC control system)
        // communication timeout of 6s on the side of BMS (6s of not receiving the status update from)
        // Charging Shutdown Emer button Should Cutout only HV or HV + Signal + LV power , if the

        // uint8_t swverH = canMsgRec.data[5];
        // uint8_t swverL = canMsgRec.data[6];
        // uint8_t hwver = canMsgRec.data[7];
      }
    }
  
  }


}
