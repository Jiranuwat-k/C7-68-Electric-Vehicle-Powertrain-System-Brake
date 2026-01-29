#include <Arduino.h>
#include <ESP32Servo.h> 

#define ESC_PIN 16 
#define SENSOR_PIN 32

Servo motor; 

// Sensor Variable
const float OffSet = 0.320;
const float Slope = 400;
const float ZeroFactor = 0;
const float SpanFactor = 1;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  motor.setPeriodHertz(50); 
  motor.attach(ESC_PIN, 1000, 2000);

  // --- Automatic Arming Sequence / Calibration Signal ---
  // send 1000us (Min) Arm ESC 
  Serial.println("Arming ESC (Sending Min Value)...");
  motor.writeMicroseconds(1000); 
  delay(2000); // wait ESC Ready
  Serial.println("ESC Armed & Ready.");

  Serial.println("--- ESC Control & Water Pressure Sensor ---");
  Serial.println("Commands: Type PWM value (1000-2000) to control motor");
}

void loop() {
  // Control Motor via Serial
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() > 0) {
      int val = input.toInt();
      
      if (val >= 800 && val <= 2200) {
        motor.writeMicroseconds(val);
        Serial.print("Set Motor PWM: ");
        Serial.println(val);
      } else {
        Serial.println("Invalid Value! (Range: 800 - 2200)");
      }
    }
  }

  // interval 500ms
  if (millis() - lastUpdate > 500) {
    lastUpdate = millis();

    // Average 20 samples
    int sumADC = 0;
    int samples = 20;
    for (int i = 0; i < samples; i++) {
      sumADC += analogRead(SENSOR_PIN);
      delay(2);
    }
    int adcValue = (int)(sumADC / samples);
    // ADC to Voltage
    float Vout = adcValue * 3.3 / 4095.0;
    // Voltage Divider Ratio
    float Vin = Vout * 31.0 / 20.0;
    // Pressure Calculation
    float Pressure = (Vin - (OffSet + ZeroFactor)) * Slope * SpanFactor;
    
    Serial.println("**************************************");
    Serial.print("[Sensor] ADC : ");
    Serial.println(adcValue);
    Serial.print("Vin : ");
    Serial.println(Vin, 3);
    Serial.print("Vout : ");
    Serial.println(Vout, 3);
    Serial.print("Pressure : ");
    Serial.println(Pressure, 3);
  }
}