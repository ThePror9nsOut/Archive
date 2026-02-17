#include <LiquidCrystal.h>
#include <stdlib.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int jumpButton = 13;
bool jumpButtonState;
bool jumped = false;

int birdPos[2]           = {0,0};
int birdBlock[2]         = {4,0};
const float birdSpeed[2] = {0,0.5};
float birdVelocity[2]    = {0,0};
const float jumpSpeed    = 3;
const float maxVelocity  = 1;

const int numOfPipes = 3;
const int pipeOffset = 5;
int pipes[numOfPipes];

int time, lastTime, jump, lastJump, pipeScroll, lastPipeUpdated, lastPipeScored = 0;
int lastBirdX = -1;
int lastBirdY = -1;
int lastPipePos[numOfPipes];

int score = 0;

  // Characters
byte bird[3] = {B10110,
                B01111,
                B00110};

byte embeddedBird[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};

byte currentPipe[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};

void updatePipe(int index) {
  pipes[index] = rand() % 8+1;

  for (int block = 0; block < 2; block++) {
    for (int row = 0; row < 8; row++) {
      if ((block == 0 && row < pipes[index]) || (block == 1 && row > 7-(8-pipes[index]))) {
        currentPipe[row] = B11111;
      }
      else {
        currentPipe[row] = B00000;
      }
    }

    lcd.createChar(1+index*2+block, currentPipe);
  }
}

void initializePipes() {
  for (int i = 0; i < numOfPipes; i++) {
    srand(i);
    pipes[i] = rand() % 8+1;
  }

  for (int pipe = 0; pipe < numOfPipes; pipe++) {
    updatePipe(pipe);

  }
}

void setup() {
  lcd.begin(16, 2);
  lcd.noAutoscroll();
  lcd.noCursor();

  pinMode(jumpButton, INPUT);

  srand(millis()*100);


  initializePipes();
}

void loop() {
  time       = millis()/75;
  jump       = millis()/675;
  pipeScroll = millis()/400;

  jumpButtonState = digitalRead(jumpButton);

  if (jumpButtonState == HIGH && jumped == false) {
    birdVelocity[1] -= jumpSpeed;
    jumped = true;
  } else if (jumpButtonState == LOW) {
    jumped = false;
  }

  if (lastTime < time) {
    lastTime = time;

    // WRAPING
    if (birdPos[0] <= 0-3) {
      if (birdBlock[0] > 0) {
        birdBlock[0] -= 1;
        birdPos[0] = 5-3;
      }
    }
    else if (birdPos[0] >= 5) {
      if (birdBlock[0] < 15) {
        birdBlock[0] += 1;
        birdPos[0] = 0;
      }
    }


    if (birdPos[1] <= 0-3) {
      if (birdBlock[1] == 1) {
        birdBlock[1] -= 1;
        birdPos[1] = 8-3;
      }
    }
    else if (birdPos[1] >= 8) {
      if (birdBlock[1] == 0) {
        birdBlock[1] += 1;
        birdPos[1] = 0;
      }
    }
    
    for (int i = 0; i < 8; i++) {
      if (i >= birdPos[1] and i < birdPos[1]+3) {
        embeddedBird[i] = bird[i-birdPos[1]];
      }
      else {
        embeddedBird[i] = B00000;
      }
    }

    birdVelocity[1] = birdVelocity[1]+birdSpeed[1] < maxVelocity ? birdVelocity[1]+birdSpeed[1] : maxVelocity;
    birdPos[1] += birdVelocity[1];

    lcd.createChar(0, embeddedBird);

    if (lastBirdX != -1)
    {
      lcd.setCursor(lastBirdX, lastBirdY);
      lcd.write(' ');
    }

    for (int pipe = 0; pipe < numOfPipes; pipe++) {
      int pos = pipe*pipeOffset - pipeScroll;
      pos = (pos % 16 + 15) % 16;


      if (pos == birdBlock[0]) {
        if (birdPos[1] < pipes[pipe] && birdBlock[1] == 0 || birdPos[1]+3 > pipes[pipe] && birdBlock[1] == 1) {
          score = 0;
          lastPipeScored = pipe;
        }
      }
      else if (pos < birdBlock[0] && pipe != lastPipeScored) {
        score++;
        lastPipeScored = pipe;
      }
      else if (pos == 15 && pipe != lastPipeUpdated) {
        updatePipe(pipe);
        lastPipeUpdated = pipe;
      }

      if (lastPipePos[pipe] != -1)
      {
        lcd.setCursor(lastPipePos[pipe], 0);
        lcd.write(' ');
        lcd.setCursor(lastPipePos[pipe], 1);
        lcd.write(' ');
      }

      lcd.setCursor(pos, 0);
      lcd.write(byte(1+pipe*2));
      lcd.setCursor(pos, 1);
      lcd.write(byte(2+pipe*2));

      lastPipePos[pipe] = pos;
    }

    lcd.setCursor(0, 0);
    lcd.print(score);

    lcd.setCursor(birdBlock[0], birdBlock[1]);
    lcd.write(byte(0));

    lastBirdX = birdBlock[0];
    lastBirdY = birdBlock[1];
  }
}