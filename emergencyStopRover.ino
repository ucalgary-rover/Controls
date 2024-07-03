#Code by Sajan Manivannan
#include <SoftwareSerial.h>
// note that the 4664 ultrasonic sensor datasheet has TX and RX holes mixed up
int RX = 10;
int TX = 11;

unsigned char dataBytes[4] = {0};

int distance = 0;

unsigned char CS;

SoftwareSerial mySerial(RX, TX);
// function definitions
float safeDistCalc(float Currentvelocity);
void brakeApply();
void distCheck(float distance);

float objDist;// objDist is the sensed/calculated distance

const float grav = 9.81; // gravity value -> m/s^2
const float fric = 0.4; // friction coefficent

//float currentVelocity = 0.56; // the max speed possible by the rover(41rotations/min * 0.82m/rotation = 33m/min = 0.56m/s) (41r/min is from NEMA23 BDCM datasheet) (0.82 is circumference of wheel aka one rotatation)
float exMargin = 0.06;// little buffer added to set dangerZone(add 6cm)
float safeDist = 0.04;// originally had a function to calculate live safeDist based off current speed but new rover goes turtlespeed so just using max speed to calculate it
                      //safeDist = (currentVelocity*currentVelocity)/(2*grav*fric);
float dangerZone = safeDist + exMargin; // first safety layer, if objectdistance is less than this value a notification is sent to driver
float emergencyZone = safeDist; // last safety layer, if objectdistance is less than this value then a notification is sent in all caps and hopefully driver clutches up

void setup() { // does all the pin setup and shii
  // Set up serial monitor(laptop-arduino connection)
  Serial.begin(19200);
  // Set up software serial port(ultrasonic sensor-arduino connection)
  mySerial.begin(9600);
}

void loop() {
  //ultrasonic sensor does its thing 
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
        objDist = 0.1*((dataBytes[1] << 8) + dataBytes[2]);
        // Print to serial monitor
        Serial.println("Distance: ");
        Serial.println(objDist);

        distCheck(objDist/100); // calls the function that checks which safety response should be carried out. should be in meters so divde by 100
      }
    } 
  } 
}

void distCheck(float senseDistance){
  if((senseDistance >= emergencyZone)&&(senseDistance < dangerZone)) { // distance is greater than emergencyZone but less than dangerZone
    //slight danger zone
    Serial.print("Warning: Object sensed, proceed with CAUTION");
  } else if(senseDistance < emergencyZone){ // distance is less than emergencyZone
    //emergency zone
    Serial.print("WARNING: OBJECT SENSED CLOSE");
  } else {
    //chillin zone
  }
}
