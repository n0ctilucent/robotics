#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Motor Pin Definitions (4 Motors, 3 Pins Each)
// Motor 1 (Chip A)
const int M1_PWM = 13; const int M1_IN1 = 12; const int M1_IN2 = 14;
// Motor 2 (Chip A)
const int M2_PWM = 27; const int M2_IN1 = 26; const int M2_IN2 = 25;
// Motor 3 (Chip B)
const int M3_PWM = 33; const int M3_IN1 = 32; const int M3_IN2 = 18;
// Motor 4 (Chip B)
const int M4_PWM = 19; const int M4_IN1 = 23; const int M4_IN2 = 5;

// PWM Properties for ESP32
const int freq = 5000;
const int resolution = 8; // 0-255 speed range

void setup() {
  Serial.begin(115200);

  // 1. Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  updateOLED("SYSTEM READY", "Wait for Test...");
  delay(2000);

  // 2. Initialize Motor Pins
  int motorPins[] = {M1_PWM, M1_IN1, M1_IN2, M2_PWM, M2_IN1, M2_IN2, 
                     M3_PWM, M3_IN1, M3_IN2, M4_PWM, M4_IN1, M4_IN2};
  
  for(int i=0; i<12; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  // 3. Setup PWM Channels on ESP32
  ledcAttach(M1_PWM, freq, resolution);
  ledcAttach(M2_PWM, freq, resolution);
  ledcAttach(M3_PWM, freq, resolution);
  ledcAttach(M4_PWM, freq, resolution);
}

void loop() {
  // TEST 1: FORWARD + SPEED RAMP UP
  updateOLED("FORWARD", "Ramping Up...");
  setDirection(true); // Set all motors to forward logic
  
  for(int speed = 0; speed <= 255; speed += 5) {
    applySpeed(speed);
    delay(50);
  }
  
  delay(1000);

  // TEST 2: BACKWARD + SPEED RAMP DOWN
  updateOLED("BACKWARD", "Ramping Down...");
  setDirection(false); // Set all motors to backward logic
  
  for(int speed = 255; speed >= 0; speed -= 5) {
    applySpeed(speed);
    delay(50);
  }

  updateOLED("STOPPED", "Restarting...");
  delay(2000);
}

// Helper: Update OLED Display
void updateOLED(String status, String detail) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("MOTOR CONTROLLER v1");
  display.drawLine(0, 12, 128, 12, SSD1306_WHITE);
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.println(status);
  
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println(detail);
  display.display();
}

// Helper: Set Motor Directions
void setDirection(bool forward) {
  if(forward) {
    digitalWrite(M1_IN1, HIGH); digitalWrite(M1_IN2, LOW);
    digitalWrite(M2_IN1, HIGH); digitalWrite(M2_IN2, LOW);
    digitalWrite(M3_IN1, HIGH); digitalWrite(M3_IN2, LOW);
    digitalWrite(M4_IN1, HIGH); digitalWrite(M4_IN2, LOW);
  } else {
    digitalWrite(M1_IN1, LOW);  digitalWrite(M1_IN2, HIGH);
    digitalWrite(M2_IN1, LOW);  digitalWrite(M2_IN2, HIGH);
    digitalWrite(M3_IN1, LOW);  digitalWrite(M3_IN2, HIGH);
    digitalWrite(M4_IN1, LOW);  digitalWrite(M4_IN2, HIGH);
  }
}

// Helper: Apply PWM Speed
void applySpeed(int speed) {
  ledcWrite(M1_PWM, speed);
  ledcWrite(M2_PWM, speed);
  ledcWrite(M3_PWM, speed);
  ledcWrite(M4_PWM, speed);
}
