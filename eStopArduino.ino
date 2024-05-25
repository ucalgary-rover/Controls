#include <SoftwareSerial.h>
// note that the 4664 ultrasonic sensor datasheet has TX and RX holes mixed up
int RX = 10;
int TX = 11;

const int yel = 5; // pin for yellow led(test purposes)
const int red = 4; // pin for red led

unsigned char dataBytes[4] = {0};

int distance = 0;

unsigned char CS;

SoftwareSerial mySerial(RX, TX);
// function definitions
float safeDistCalc(float Currentvelocity);
void brakeApply();
void distCheck(float distance);

float duration, objDist;// duration is ____
                        // objDist is the sensed/calculated distance

const float grav = 9.81; // gravity value -> m/s^2
const float fric = 0.4; // friction coefficent

float roverSetSpeed; // variable that will set the new rover Speed after brakes are being applied
float currentSpeed = 1.75; // current speed of rover that is measured from idk where, rn i put 1.75 so that the dangerzone is ~39cm
float exMargin = 0.075; // little buffer added to make sure rover stop with room for error(add 7.5cm)
float safeDist; //distance rover will take to stop --> calculated in function safeDistCalc
float dangerZone; // first safety layer, if objectdistance is less than this value a notification is sent to driver
float emergencyZone; // last safety layer, if objectdistance is less than this value then a notification is sent and brakes are applied
int emergencyPauseTime = 3000; // like a pause time after the emergency brake has been applied to like recollect and calm down 


void setup() { // does all the pin setup and shii
  // Set up serial monitor(laptop-arduino connection)
  Serial.begin(19200);
  // Set up software serial port(ultrasonic sensor-arduino connection)
  mySerial.begin(9600);

  pinMode(yel, OUTPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  safeDist = safeDistCalc(currentSpeed); //calculate the rover stopping distance
  dangerZone = safeDist + exMargin; // dangerzone distance setup
  emergencyZone = safeDist; // emergency zone distance setup ***might be ideal to add a small margin to this one as well to make more effective
  
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

float safeDistCalc(float currentVelocity){
	safeDist = (currentVelocity*currentVelocity)/(2*grav*fric); //uses a kinematics eqn i found that i think calculates the stopping distance of the rover
	return safeDist;
}

//applies brakes. not exactly sure how that will work with the actual rover driving code but that can be figured out later
void brakeApply() { 
	roverSetSpeed = 0; // zero or negative max speed if thats safe for the rover
	if(currentSpeed==0) {
		delay(emergencyPauseTime); // pause for like 3 seconds
		roverSetSpeed = 55; // basically should go back to listening to input from driver
	}
}

void distCheck(float senseDistance){
  if((senseDistance >= emergencyZone)&&(senseDistance < dangerZone)) { // distance is greater than emergencyZone but less than dangerZone
    digitalWrite(yel, HIGH);
    digitalWrite(red, LOW);
  } else if(senseDistance < emergencyZone){ // distance is less than emergencyZone
    brakeApply(); //apply the brakes
    digitalWrite(yel, LOW);
    digitalWrite(red, HIGH);
  } else {
    digitalWrite(yel, LOW);
    digitalWrite(red, LOW);
  }
}
