/*
This code just prints out the distance sensed by the adafruit 4664 ultrasonic sensor using UART
Figured useful enough to just leave here for the future
*/

#include <SoftwareSerial.h>

// the datasheet was written by a dumbass cuz the RX and TX are switched
// RX is supposed to be wired as 4 and TX as 3 (VCC - 1, GND - 2)
int RX = 10;
int TX = 11;
 
// the data is formatted with 4 bytes... 
// the FIRST byte being dedicated to symbolizing the beggining of the serial data (Header), the first byte is 0xff so the code will know its reading a new data everytime it reads 0xff
// the SECOND byte is upper first of the bits representing the distance (mm)
// the THIRD byte is the second half of the bits representing the distance (mm)
// the FOURTH byte is supposed to validate the data, it SHOULD be equal to the first 3 bits added together (checksum)

// an array to store the incoming serial data
unsigned char dataBytes[4] = {0};
 
// integer to store distance
int distance = 0;
 
// variable to hold checksum
unsigned char CS;
 
// this is like a 2nd serial communication "link(?)" specifically for communication between the sensor and arduino, the 1st is regular Serial which is arduino and user laptop(i think)
// Object to represent software serial port
// SoftwareSerial mySerial(pinRX, pinTX);
SoftwareSerial mySerial(RX, TX);
 
void setup() {
  // Set up serial monitor(laptop-arduino connection)
  Serial.begin(19200);
  // Set up software serial port(ultrasonic sensor-arduino connection)
  mySerial.begin(9600);
}
 
void loop() {
  // checks to see if theres data available to be read and will run rest of the code if that is true
  if (mySerial.available() > 0) {
    delay(4);
    // Check for serial data header character 0xff to indicate begginning of serial data segment
    if (mySerial.read() == 0xff) {
      // Insert header into array
      dataBytes[0] = 0xff;
      // Read remaining 3 characters of data and insert into array
      for (int i = 1; i < 4; i++) {
        dataBytes[i] = mySerial.read();
      }
      //calculate checksum
      CS = dataBytes[0] + dataBytes[1] + dataBytes[2];
      // if checksum is valid then data is valid so we can determine the distance from the data
      if (dataBytes[3] == CS) {
        //left shit the higherbits(second byte) then add the lower bits(third byte)
        distance = (dataBytes[1] << 8) + dataBytes[2];
       // Print to serial monitor
        Serial.print("distance: ");
        Serial.print(distance/10); // divide for cm
        Serial.println(" cm");
      }
    } 
  } else {
    Serial.print("ERR: No Serial Data able to be read") 
}
