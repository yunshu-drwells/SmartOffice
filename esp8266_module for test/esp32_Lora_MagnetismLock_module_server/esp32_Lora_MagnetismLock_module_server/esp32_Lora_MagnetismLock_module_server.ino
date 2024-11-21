#include <Arduino.h>

#define RELAY_PIN 0

// 指定的消息
const char* expectedMessage0 = "Open_Door";

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  //digitalWrite(RELAY_PIN, LOW);
  Serial.println("MagnetismLock init down!");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    Serial.println("Received:");
    Serial.println(input);
    // 检查串口接收到的信息
    if (input == expectedMessage0) {
      digitalWrite(RELAY_PIN, LOW); // 拉高IO0口电平
      Serial.println("Door opened!");
      delay(10000); // 延时10秒
      digitalWrite(RELAY_PIN, HIGH); // 恢复IO0口电平
      Serial.println("Door closed!");
    }
  }
}
