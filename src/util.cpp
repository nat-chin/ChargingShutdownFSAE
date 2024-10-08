#include <iostream>
#include <cstdint>

// Split and merge High byte low byte of 16 bit unsigned integer
unsigned char *splitHLbyte(float num){
  static unsigned char temp[2] = {0,0}; // initialize
  
  return temp;
}
// HighByte = splitHLbyte[0]
// LowByte = splitHLbyte[1]
float mergeHLbyte(unsigned char Hbyte, unsigned char Lbyte){
  static float temp = 0;
  // bitshiftLeft by 8 AND with 1, then put to 2 different variable
  return temp;
}

// Encode float or uint16 into arrays of uint8
unsigned char *Encode_bytearray(float f) { 
    // Use memcpy to copy the bytes of the float into the array
    static unsigned char c[sizeof(f)]; 
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

// Split and Check bit from MSB -> LSB
unsigned char *checkMSB(unsigned char num){
  // uint8_t num = 42;  // Example decimal number (42)
  static unsigned char arr[8]; // array to hold 8 binary number
    for (int i = sizeof(uint8_t) * 7; i >= 0; i--)
        arr[7-i] = (num & (1 << i)) >> i ;
        // arr[7-7] = arr[0] = num & ..
        // printf("%d", (num & (1 << i)) >> i );
        
    // putc('\n', stdout);
    return arr;
}

// Split and Check bit from LSB -> MSB
unsigned char *checkLSB(unsigned char num){
  for (int i = sizeof(char) * 7; i >= 0; i--)
        printf("%d", (num & (1 << i)) >> i );
    // putc('\n', stdout);
}

// Test & Debug

int main() {
    // uint8_t num = 42;  // Example decimal number (42)
    // for (int i = sizeof(uint8_t) * 7; i >= 0; i--)
    //     printf("%d", (num & (1 << i)) >> i );
    // putc('\n', stdout);
    
    return 0;
}