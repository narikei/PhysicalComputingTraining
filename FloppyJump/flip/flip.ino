#include <Keyboard.h>

const int analogPressPin = 1;
const int soundPin = 10;

boolean direction = true;
int values[5] = {1024, 1024, 1024, 1024, 1024};

void setup(){
  Serial.begin(9600);  // シリアル通信速度
  Keyboard.begin(); // キーボード開始
  pinMode(soundPin, OUTPUT);
}

void loop(){
  for (int i = 0; i < 5; i++) {
    values[i] = values[i+1];
  }
  values[4] = analogRead(analogPressPin);

  // 平均値
  double value = 0;
  for (int i = 0; i < 5; i++) {
    value = value + values[i];
  }
  value = value / 5;

  Serial.println("+");
  Serial.println(value);                 

  // ジャンブ
  if (value < 25) {
    Keyboard.press(' ');
    tone(soundPin, 440, 100);
    delay(100);
    Keyboard.releaseAll();
  }

  delay(50);
}

