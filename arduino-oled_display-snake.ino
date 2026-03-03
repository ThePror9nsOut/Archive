#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1106G display(128, 64, &Wire, -1);

void setup() {
  if (!display.begin(0x3C, true)) {
    while (1)
      ;
  }

  pinMode(12,INPUT);
  pinMode(11,INPUT);

  randomSeed(analogRead(0));

  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);

  clean_tail();
  relocate_apple();
}

struct vector2 {
  u8 x;
  u8 y;
};

vector2 tail[128/4];
int tail_size = 1;
int high_score = 0;

vector2 head = {64,32};
vector2 apple = {0,0};

u8 spd = 0;

bool press1 = false;
bool press2 = false;

int ltime = 0;

void clean_tail() {
  for (int i = 0; i < 128/4; i++) {
    tail[i] = {0,64};
  }
}

void shift_tail() {
  if (tail_size <= 1) return;

  for (int i = tail_size - 1; i > 0; i--) {
    tail[i] = tail[i - 1];
  }
}


void reset() {
  head.x = 64;
  head.y = 32;
  tail_size = 1;
  relocate_apple();
}

void relocate_apple() {
  apple = {random(128-8),random(64-8)};
  apple.x = apple.x - apple.x%4;
  apple.y = apple.y - apple.y%4;
}

void draw() {
  display.clearDisplay();

  display.drawRect(0, 0, 128, 64, SH110X_WHITE);

  display.fillRect(apple.x, apple.y, 4, 4, SH110X_WHITE);

  for (int i = 0; i < tail_size; i ++) {
    display.fillRect(tail[i].x, tail[i].y, 4, 4, SH110X_WHITE);
  }

  display.fillRect(head.x, head.y, 4, 4, SH110X_WHITE);

  display.setCursor(apple.x < 60 && apple.y < 30 ? 86 : 1, 1);
  display.print("s:"); display.print(tail_size-1);
  display.print(" h:"); display.print(high_score);

  display.display();
}

void loop() {
  int time = millis()/200;

  if (ltime < time) {
    ltime = time;
    
    if (spd == 0) {
      head.x += 4;
    }
    else if (spd == 1) {
      head.y += 4;
    }
    else if (spd == 2) {
      head.x -= 4;
    }
    else if (spd == 3) {
      head.y -= 4;
    }

    for (int i = 0; i < tail_size; i++) {
      if (tail[i].x == head.x && tail[i].y == head.y) {
        reset();
      }
    }

    if (head.x < 0 || head.x >= 128 || head.y < 0 || head.y >= 64) {
      reset();
    }

    if (head.x == apple.x && head.y == apple.y) {
      tail_size++;

      if (high_score < tail_size-1) {
        high_score = tail_size-1;
      }

      while (head.x == apple.x && head.y == apple.y) {
        relocate_apple();
      }

      bool intail = true;

      while (intail) {
        intail = false;
        for (int i = 0; i < tail_size; i++) {
          if (apple.x == tail[i].x && apple.y == tail[i].y) {
            intail = true;
            break;
          }
        }

        relocate_apple();
      }
    }

    shift_tail();
    tail[0] = head;

    draw();
  }

  if (digitalRead(11) == HIGH) {
    if (!press1) {
      spd = spd <= 0 ? 3 : spd - 1;
      press1 = true;
    }
  }
  else {
    press1 = false;
  }

  if (digitalRead(12) == HIGH) {
    if (!press2) {
      spd = spd >= 3 ? 0 : spd + 1;
      press2 = true;
    }
  }
  else {
    press2 = false;
  }
}

