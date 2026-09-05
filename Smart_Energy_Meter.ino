#define BLYNK_TEMPLATE_ID "TMPL3TyZKSX41"
#define BLYNK_TEMPLATE_NAME "IoT Smart Energy Meter"
#define BLYNK_AUTH_TOKEN "3A0So9Mliul1hokIK8hvzeRdxza2uTNn"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "FAB.NET5G";
char pass[] = "satabdi@123";

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ACS_PIN = 34;
const float sensitivity = 0.066;   // ACS712 30A = 66mV/A

float zeroOffset = 0;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Calibrating...");

  pinMode(ACS_PIN, INPUT);

  // Zero-current calibration
  long sum = 0;
  for(int i=0; i<500; i++){
    sum += analogRead(ACS_PIN);
    delay(2);
  }
  zeroOffset = (sum / 500.0) * (3.3 / 4095.0);

  lcd.clear();
  lcd.print("Connecting...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.clear();
}

void loop() {
  Blynk.run();

  int adc = analogRead(ACS_PIN);
  float sensorVoltage = adc * (3.3 / 4095.0);

  float current = (sensorVoltage - zeroOffset) / sensitivity;

  // Remove small noise
  if (abs(current) < 0.05)
    current = 0;

  current = abs(current);

  lcd.setCursor(0,0);
  lcd.print("S:");
  lcd.print(sensorVoltage,2);
  lcd.print("V   ");

  lcd.setCursor(0,1);
  lcd.print("I:");
  lcd.print(current,2);
  lcd.print("A   ");

  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V0, sensorVoltage);

  Serial.print("Sensor Voltage: ");
  Serial.print(sensorVoltage,2);
  Serial.print(" V | Current: ");
  Serial.print(current,2);
  Serial.println(" A");

  delay(300);
}