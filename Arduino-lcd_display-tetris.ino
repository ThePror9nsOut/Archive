#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int SECTIONW = 8;
const int CHARLIMIT = 8;
const int DRAWLIMIT = 5;

struct vector2 {
  int x;
  int y;
};

struct cell {
  byte bytes[8];
};

cell *cells = (cell*)calloc(2*SECTIONW,sizeof(cell));

byte blank_cell[8] = {0,0,0,0,0,0,0,0};
byte full_cell[8] = {31,31,31,31,31,31,31,31};
byte crossed_cell[8] = {B10001,B01010,B01010,B00100,B00100,B01010,B01010,B10001};

byte current_figure[8] = {B10000,
                          B11100};
int figure_pos[2] = {0,4};
int last_figure_pos[2] = {0,0};

int drawing_offset = 10;
int peak = 0;

bool collision_flag = false;

void draw() {
  int figure_cell = figure_pos[0]/5;

  // CELL RENDER FROM ARRAY
  for (int c = 0; c < DRAWLIMIT; c++) {
    int offset = c+drawing_offset;

    if (offset == figure_cell) {
      continue;
    }

    lcd.createChar(c, cells[offset].bytes);

    lcd.setCursor(offset%8, offset>=8);
    lcd.write(byte(c));
  }


  // OUTSIDE RENDER FIELD MARKING
  for (int c = 0; c < SECTIONW-drawing_offset+2; c++) {
    lcd.setCursor((drawing_offset+DRAWLIMIT+c)%8, (drawing_offset+DRAWLIMIT+c)/8);
    lcd.write(byte(6));
  }


  // CLEAR, DRAW, MANAGE FALLING FIGURE'S COLLISION
  if (last_figure_pos[0] != figure_cell%8 && (last_figure_pos[0] < drawing_offset || last_figure_pos[0] > drawing_offset+DRAWLIMIT) && !collision_flag) {
    lcd.setCursor(last_figure_pos[0], last_figure_pos[1]);
    lcd.print(" ");

    last_figure_pos[0] = figure_cell%8;
    last_figure_pos[1] = figure_cell/8;
  }

  byte transformed_figure[8] = {};
  byte additional_figure[8] = {};

  for (int i = 0; i < 8; i++) {
    if (i >= figure_pos[1]) {
      byte fig_strip = current_figure[i-figure_pos[1]];
      byte transformed_strip = (fig_strip >> figure_pos[0]%5);


      // If after the move the figure's strip is smaller than it was...
      if ((transformed_strip << figure_pos[0]%5) < fig_strip) {
        // ...and if there are any bytes in the cell,
        // update the drawn cell and check collision flag
        if (cells[figure_cell].bytes[i] > 0) {
          byte new_strip = fig_strip | cells[figure_cell].bytes[i];
          transformed_figure[i] = new_strip;

          cells[figure_cell].bytes[i] = new_strip;
          collision_flag = true;

          continue;
        }
        // ...or the figure just goes out of bounds
        else {
          transformed_figure[i] = fig_strip;

          additional_figure[i] = fig_strip & (transformed_strip << figure_pos[0]%5);
        }
      }

      transformed_figure[i] = transformed_strip | cells[figure_cell].bytes[i];
    }
    else {
      transformed_figure[i] = cells[figure_cell].bytes[i];
    }
  }

  if (collision_flag) {
    figure_pos[0] = 0;
    // figure_pos[1] = 0;
    last_figure_pos[0] = 0; last_figure_pos[1] = 0;
    collision_flag = false;
  }

  lcd.createChar(6, transformed_figure);

  if (additional_figure) {
    lcd.createChar(7, additional_figure);
  }

  lcd.setCursor(figure_cell%8, figure_cell/8);
  lcd.write(byte(6));
  lcd.setCursor(figure_cell%8+1, figure_cell/8);
  lcd.write(byte(7));
}

void setup() {

  lcd.begin(8, 2);
  lcd.clear();

  for (int i = 0; i < 16; i++) {
    replace_cell(i,blank_cell);
  }

  replace_cell(10,crossed_cell);

  lcd.createChar(5,crossed_cell);
  lcd.createChar(6,current_figure);
}

void replace_cell(int c, byte n[]) {
  for (int l = 0; l < 8; l++) {
      cells[c].bytes[l] = n[l];
  }
}

int fs = 0;

void loop() {
  int sc = millis()/500;

  if (fs < sc) {
    figure_pos[0]++;

    draw();

    fs = sc;
  }

  delay(100);
}