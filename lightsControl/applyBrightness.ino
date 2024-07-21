// coded by sajan and chatgpt
// headlight controlling pin if thats even neccessary but im using it for my testing purposes
int headlightPin = 9;

// Brightness levels --> 0%, 25%, 50%, 100% Brightnesses
int brightnessLevels[] = {0, 64, 128, 255}; 
int brightVal[] = {0, 25, 50, 100};

void setup() {
  // headlight pin is OUTPUT
  pinMode(headlightPin, OUTPUT);

  // headlight initially turned off 
  analogWrite(headlightPin, 0);

  // initialize serial communication at 9600 baud
  Serial.begin(9600);
}

void loop() {
  // checks if there is data available to read
  if (Serial.available() > 0) {
    // read the incoming byte
    int incomingByte = Serial.read();

    // determine the brightness level based on the received byte, extracting last 2 bits
    int brightnessIndex = incomingByte & 0x03;

    // Set the brightness based off received data
    analogWrite(headlightPin, brightnessLevels[brightnessIndex]);

    // message sending the set brightness back
    String message = "Headlight Brightness Level: " + String(brightVal[brightnessIndex]) +"%";
    Serial.println(message);
  }
}
