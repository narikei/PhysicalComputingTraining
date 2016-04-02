#include <Keyboard.h>

const int analogPressPin = 1;
const int buttonPin = 2;

int jump_key_enable = 0; // キー入力フラグ
int key_enable = 0; // キー入力フラグ
int count = 0;
int buttonState = 0;
boolean direction = true;
int values[5] = {1024, 1024, 1024, 1024, 1024     };

void setup(){
  Serial.begin(9600);  // シリアル通信速度
  Keyboard.begin(); // キーボード開始
  pinMode(buttonPin, INPUT);
  pinMode(10, OUTPUT);
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
    if (jump_key_enable == 0) {
      jump_key_enable = 1;
      Keyboard.press(' ');
      tone(10, 440, 200);
      delay(50);
    }
  } else {
    jump_key_enable = 0;
    Keyboard.release(' ');
  }

  if (digitalRead(buttonPin) == HIGH) {
    if (key_enable == 0) {
      direction = !direction;
      Keyboard.release(KEY_LEFT_ARROW);
      Keyboard.release(KEY_RIGHT_ARROW);
      
      if (direction) {
        Keyboard.press(KEY_LEFT_ARROW);    
      } else {
        Keyboard.press(KEY_RIGHT_ARROW);    
      }
   }
  }

  delay(50);
}






