const int trigPin = 9; // pin for trigpin -- tx equivalent(i think) 
const int echoPin = 10; // pin for echopon -- rx equivalent 

const int yel = 5; // pin for yellow led(test purposes)
const int red = 4; // pin for red led

// function definitions
float safeDistCalc(float Currentvelocity);
void brakeApply();
void distCheck(float distance);

float duration, objDist;// duration is ____
                        // objDist is the sensed/calculated distance

const float grav = 9.81; // gravity value -> m/s^2
const float fric = 0.4; // friction coefficent

float roverSetSpeed; // variable that will set the new rover Speed after brakes are being applied
float currentSpeed = 1.0; // current speed of rover that is measured from idk where
float exMargin = 0.025; // little buffer added to make sure rover stop with room for error
float safeDist; //distance rover will take to stop --> calculated in function safeDistCalc
float dangerZone; // first safety layer, if objectdistance is less than this value a notification is sent to driver
float emergencyZone; // last safety layer, if objectdistance is less than this value then a notification is sent and brakes are applied
int emergencyPauseTime = 3000; // like a pause time after the emergency brake has been applied to like recollect and calm down 


void setup() { // does all the pin setup and shii
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(yel, OUTPUT);
    pinMode(red, OUTPUT);
    Serial.begin(4800);
}

void loop() {
    //ultrasonic sensor does its thing 
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    safeDist = safeDistCalc(currentSpeed); //calculate the rover stopping distance
    dangerZone = safeDist + exMargin; // dangerzone distance setup
    emergencyZone = safeDist; // emergency zone distance setup ***might be ideal to add a small margin to this one as well to make more effective

    duration = pulseIn(echoPin, HIGH); //im not too sure but i think it finds the TIME it takes the ultrasonic sound to go to the object and bounce back
    objDist = (duration*.0343)/(2*100); // the time is then converted to distance -- i think its distance = vt = (speed of sound)(time to/from object)/200 -- the 100 is to convert to meters
    distCheck(objDist); // calls the function that checks which safety response should be carried out
   
    Serial.println("Distance:"); // print the distance
    Serial.println(objDist*100);

    // Serial.println("emergency:");
    // Serial.print(emergencyZone*100);

    // Serial.println("danger:");
    // Serial.print(dangerZone*100);

    // Serial.println("----------------------------");
    
    //delay(100); // i think i added this to control how fast the serial monitor updated
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
