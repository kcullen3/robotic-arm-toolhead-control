#include "ClearCore.h"

#define baudRate 9600
#define SerialPort ConnectorUsb

#define relay IO2 
#define steps IO1
#define dir IO0
#define analogSig A12

void setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  delay(1000);  //Wait for devices to power

  pinMode(steps, OUTPUT);
  pinMode(dir, OUTPUT);
  digitalWrite(dir, LOW); // Initial direction = extrude
  pinMode(analogSig, INPUT);

// Serial port Setup:
  SerialPort.Mode(Connector::USB_CDC);
  SerialPort.Speed(baudRate);
  uint32_t timeout = 5000;
  uint32_t startTime = Milliseconds();
  SerialPort.PortOpen();
  while (!SerialPort && Milliseconds() - startTime < timeout) {continue;}
}

const float dt = 1000.0;
float currentSpeedFactor = 1.0;
float smoothing = 5.0;

void loop() {
  float targetSpeedFactor = map(analogRead(analogSig), 0, 4097, 1.0, dt);   // Analog input = motor step length

  // Smooth interpolation, prevents acceleration/decceleration errors
  currentSpeedFactor = (currentSpeedFactor * (100.0 - smoothing) + targetSpeedFactor * smoothing)/100.0;

  // Do ONE step
  digitalWrite(steps, HIGH);
  delayMicroseconds(dt/currentSpeedFactor);
  digitalWrite(steps, LOW);
  delayMicroseconds(dt/currentSpeedFactor);

  //CANNOT ADD ANY MORE CODE
  //MORE CALCULATIONS = SLOWER STEP TIME
}