//Pin connected to ST_CP of 74HC595
int latchPin = 8;   // ※みどり
int clockPin = 12;  // ※き
int dataPin = 11;   // ※あか

const int buttonPinA = 5;
const int buttonPinB = 6;

boolean gamePlaying = false;
int speed = 200;
int dir = 1; // 0:top, 1:right, 2:bottom, 3:left
int snakeLength = 0;

byte x[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
byte y[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

int oldSnake[64][2];
int snake[64][2];
int bait[2];

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(buttonPinA,INPUT);
  pinMode(buttonPinB,INPUT);
  Serial.begin(9600);

  gameInit();
}

void loop() {
  if (gamePlaying) {
    // LED表示
    for (int s = 0; s < speed / snakeLength; s++) {
      // 蛇表示
      for (int i = 0; i < snakeLength; i++) {
        digitalWrite(latchPin, 0);
        shiftOut(dataPin, clockPin, x[snake[i][0]]);
        shiftOut(dataPin, clockPin, y[snake[i][1]]);
        digitalWrite(latchPin, 1);
        delay(1);
      }
      
      // 餌表示
      digitalWrite(latchPin, 0);
      shiftOut(dataPin, clockPin, x[bait[0]]);
      shiftOut(dataPin, clockPin, y[bait[1]]);
      digitalWrite(latchPin, 1);

      delay(5);
    }
    
    // 方向決定
    selectDir();
    
    // 蛇移動
    snakeMove();

    // 壁判定
    if (snake[0][0] < 0 || snake[0][0] > 7 || snake[0][1] < 0 || snake[0][1] > 7) {
      gameOver();
    }
    
    // 身体判定
    for (int i = 1; i < snakeLength; i++) {
      if (snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1]) {
        gameOver();
        break;
      }
    }
    
    // 餌判定
    if (snake[0][0] == bait[0] && snake[0][1] == bait[1]) {
      snake[snakeLength][0] = snake[0][0];
      snake[snakeLength][1] = snake[0][1];
      snakeLength++;
      putBait();
      speed = speed * 0.99;
    }    
  } else {
    if (digitalRead(buttonPinA) == HIGH && digitalRead(buttonPinB) == HIGH) {
      gameInit();
    }

    gameCompleat();
  }
}

void gameInit() {
  gamePlaying = true;
  dir = 1;
  snake[64][2] = {};
  
  snake[0][0] = 0;
  snake[0][1] = 0;
  snake[1][0] = 1;
  snake[1][1] = 0;
  
  snakeLength = 2;
  putBait();
}

void gameOver() {
  gamePlaying = false;

  Serial.print("score:");
  Serial.println(snakeLength);

  for (int i = 0; i < 5; i++) {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 0xFF);
    shiftOut(dataPin, clockPin, 0x00);
    digitalWrite(latchPin, 1);
    delay(50);
    
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 0x00);
    shiftOut(dataPin, clockPin, 0xFF);
    digitalWrite(latchPin, 1);
    delay(50);
  }
}

void gameCompleat() {
  for (int i = 0; i < snakeLength; i++) {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, x[oldSnake[i][0]]);
    shiftOut(dataPin, clockPin, y[oldSnake[i][1]]);
    digitalWrite(latchPin, 1);
    delay(50);
  }
}

void selectDir() {
  if (digitalRead(buttonPinA) == HIGH) {     // 時計回り
    dir = dir + 1;
  } else if (digitalRead(buttonPinB) == HIGH) {     // 反時計回り
    dir = dir - 1;
  }

  if (dir < 0) {
    dir = 3;
  }
  if (dir > 3) {
    dir = 0;
  }
}

void snakeMove() {
  int newSnake[64][2];

  oldSnake;
  for (int i = 0; i < snakeLength; i++) {
    oldSnake[i][0] = snake[i][0];
    oldSnake[i][1] = snake[i][1];
  }
  
  if (dir == 0) { // top
    newSnake[0][0] = snake[0][0];
    newSnake[0][1] = snake[0][1] - 1;
  } else if (dir == 1) { //right
    newSnake[0][0] = snake[0][0] + 1;
    newSnake[0][1] = snake[0][1];
  } else if (dir == 2) {
    newSnake[0][0] = snake[0][0];
    newSnake[0][1] = snake[0][1] + 1;
  } else {
    newSnake[0][0] = snake[0][0] - 1;
    newSnake[0][1] = snake[0][1];
  }
  
  for (int i = 0; i < snakeLength - 1; i++) {
    newSnake[i + 1][0] = snake[i][0];
    newSnake[i + 1][1] = snake[i][1];
  }

  for (int i = 0; i < snakeLength; i++) {
    snake[i][0] = newSnake[i][0];
    snake[i][1] = newSnake[i][1];
  }
}

void putBait() {
  int x;
  int y;
  
  boolean loopFlg = true;
  while (loopFlg) {
    loopFlg = false;
    x = random(0, 7);
    y = random(0, 7);
    
    for (int i = 0; i < snakeLength; i++) {
      if (x == snake[i][0] && y == snake[i][1]) {
        loopFlg = true;
        break;
      }
    }
  }
  
  bait[0] = x;
  bait[1] = y;
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  for (int i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    } else {
      pinState= 0;
    }
    
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }

  digitalWrite(myClockPin, 0);
}
