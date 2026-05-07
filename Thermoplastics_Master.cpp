#include "ClearCore.h"

// Set up Serial Monitor
#define baudRate 9600
#define SerialPort ConnectorUsb

// Clearcore INPUT signals (Analog and digital signals from ABB digital I/Os):
#define extrusionSig A10   // Start/stop extrusion signal [DO1]
#define fans1Sig DI8       // Activate 2 blower fans (24V cable prong #2) [DO4]
#define fans2Sig A9        // Activate 2 other blower fans (24V cable prong #3) [DO5]
#define analogSig A12      // Analog input to determine thermoplastic motor velocity [AO1]

//Clearcore OUTPUT signals
#define fans1PWR IO3        // Signal for 24V relay port 2 + cable prong 2 (activates 2 blower fans)
#define fans2PWR IO2        // Signal for 24V relay port 3 + cable prong 3 (activates other 2 blower fans)
#define motor ConnectorM0   // Motor logic cable (blue)

int32_t V_MAX = 30000;           // Maximum (safe) velocity the motor can handle
int32_t A_MAX = 5000;            // Maximum (safe) acceleration the motor can handle
float rpm_multiplier = 0.0187;   // Multiply with arduino value to determine motor's rpm

void setup() {
// Motor setup:   
  motor.ClearAlerts();
  MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);                      
  MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL, Connector::CPM_MODE_STEP_AND_DIR);
  motor.VelMax(V_MAX);
  motor.AccelMax(A_MAX);
  motor.EnableRequest(true);

// Serial port Setup:
  SerialPort.Mode(Connector::USB_CDC);
  SerialPort.Speed(baudRate);
  uint32_t timeout = 5000;
  uint32_t startTime = Milliseconds();
  SerialPort.PortOpen();
  while (!SerialPort && Milliseconds() - startTime < timeout) {continue;}

// Set up input signals:
  int inputPins[] = {extrusionSig, fans1Sig, fans2Sig, analogSig};  // ABB Digital Outputs = Clearcore Inputs
  for (int i = 0; i < sizeof(inputPins) / sizeof(inputPins[0]); i++) {
    pinMode(inputPins[i], INPUT);
  }

// Setup 24V DOs, ensure they start up as OFF:
  pinMode(fans1PWR, OUTPUT);
  pinMode(fans2PWR, OUTPUT);
  digitalWrite(fans1PWR, HIGH);  // Value is flipped for some reason
  digitalWrite(fans2PWR, LOW);
}

void loop() {
// Analog Motor Control:
  float V_Offset = 1.2;
  float Motor_Velocity = map(analogRead(analogSig), 0, 4095, 0, V_MAX) * V_Offset;

// Start/Stop Extrusion:
  if (analogRead(extrusionSig) > 500){motor.MoveVelocity(Motor_Velocity);}
  else {motor.MoveVelocity(0);}

// 24V Relay Control:
  if (digitalRead(fans1Sig) == 1){digitalWrite(fans1PWR, 0);}  //value is flipped for some reason
  else {digitalWrite(fans1PWR, 1);}
  if (analogRead(fans2Sig) > 500){digitalWrite(fans2PWR, 1);}  // Using analog input as digital input
  else {digitalWrite(fans2PWR, 0);}
//For testing/debugging:
  //Serial.print("Extrusion Signal: "); Serial.println(analogRead(extrusionSig));
  //Serial.println("Analog Signal: "); Serial.println(analogRead(extrusionSig));
  //Serial.print("Motor Velocity: "); Serial.println(Motor_Velocity);
  //Serial.print("Motor rpm: "); Serial.println(Motor_Velocity*rpm_multiplier);

  delay(10);   // Small delay makes code more stable
}