#include <Arduino.h>
#include <ESP32Servo.h> 

#define ESC_PIN 16 
#define SENSOR_PIN 32 // Pin สำหรับ Sensor แรงดันน้ำ (จากไฟล์ test)

Servo motor; 

// ตัวแปรสำหรับ Sensor
const float OffSet = 0.320; // ปลด const ออก เพื่อให้ปรับค่าได้ขณะ Run
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
  // ส่งสัญญาณ 1000us (Min) เพื่อ Arm ESC ให้พร้อมทำงาน
  Serial.println("Arming ESC (Sending Min Value)...");
  motor.writeMicroseconds(1000); 
  delay(2000); // รอให้ ESC ตรวจสอบสัญญาณและหยุดร้อง (Ready)
  Serial.println("ESC Armed & Ready.");

  Serial.println("--- ESC Control & Water Pressure Sensor ---");
  Serial.println("Commands: Type PWM value (1000-2000) to control motor");
}

void loop() {
  // 1. ส่วนควบคุม Motor ผ่าน Serial
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

  // 2. ส่วนอ่านค่า Sensor แรงดันน้ำ (อ่านทุกๆ 500ms)
  if (millis() - lastUpdate > 500) {
    lastUpdate = millis();

    // อ่านค่าหลายๆ รอบเพื่อหาค่าเฉลี่ย (Average) ลด Noise
    int sumADC = 0;
    int samples = 20; // จำนวนครั้งที่จะอ่าน
    for (int i = 0; i < samples; i++) {
      sumADC += analogRead(SENSOR_PIN);
      delay(2); // เว้นระยะนิดหน่อย
    }
    int adcValue = (int)(sumADC / samples); // ค่าเฉลี่ย

    float Vout = adcValue * 3.3 / 4095.0;
    float Vin = Vout * 31.0 / 20.0; // Voltage Divider
    // float P = Vin
    float Pressure = (Vin - (OffSet + ZeroFactor)) * Slope * SpanFactor;
    
    // คำนวณ Pressure (ถ้าต้องการใช้ให้ Uncomment และปรับสูตร)
    // float P = (Vin - OffSet) * 250; 
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