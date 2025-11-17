#include <TMCStepper.h>
#include <AccelStepper.h>

// ---------------- PIN DEFINITIONS ----------------
#define EN_PIN     10   // LOW = enabled
#define STEP_PIN   11
#define DIR_PIN    12

// UART Serial (Arduino Mega): RX3=15, TX3=14
HardwareSerial & UART_PORT = Serial3;

// TMC2209 Settings
#define R_SENSE        0.11
#define DRIVER_ADDRESS 0b00   // Must match MS1/MS2
TMC2209Stepper driver(&UART_PORT, R_SENSE, DRIVER_ADDRESS);

// AccelStepper driver
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
  Serial.begin(115200);
  delay(200);

  // Enable pin
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);   // ENABLE DRIVER

  // Start UART for TMC2209
  UART_PORT.begin(115200);

  // Initialize TMC2209
  driver.begin();
  driver.toff(5);              // Required to enable driver
  driver.rms_current(900);     // MOTOR CURRENT (adjust if needed)
  driver.microsteps(8);       // Good default
  driver.pwm_autoscale(true);  // Smooth stepping

  // Stepper motion settings
  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(2000);

  Serial.println("TMC2209 initialized.");
}

void loop() {
  Serial.println("Moving motor...");

  stepper.moveTo(10000);
  while (stepper.distanceToGo() != 0) stepper.run();

  delay(200);

  stepper.moveTo(-10000);
  while (stepper.distanceToGo() != 0) stepper.run();

  delay(200);
}
