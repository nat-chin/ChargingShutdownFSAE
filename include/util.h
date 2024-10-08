// Split and merge High byte low byte of 16 bit unsigned integer
unsigned char *splitHLbyte(float num);
float mergeHLbyte(unsigned char Hbyte, unsigned char Lbyte);
// Encode float or uint16 into arrays of uint8
unsigned char *Encode_bytearray(float f);
// This is for voltage monitoring
float Decode_bytearray(unsigned char* c);
// Split and Check bit from MSB -> LSB
unsigned char *checkMSB(unsigned char num);
// Split and Check bit from LSB -> MSB
unsigned char *checkLSB(unsigned char num);