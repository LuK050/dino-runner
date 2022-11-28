#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

void (* reset) (void) = 0;

const int PIN_BUTTON = 50;
const int PIN_LED_RED = 8;
const int PIN_LED_GREEN = 9;
const int PIN_BUZZER = 2;
const int FPS = 6;
const int DEFAULT_X = 2;

LiquidCrystal_I2C lcd(0x27, 20, 4);
uint8_t dinosaur[8][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x18, 0x1F},
    {0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x0F, 0x1F},
    {0x1F, 0x17, 0x1F, 0x1F, 0x1C, 0x1F, 0x1C, 0x1C},
    {0x10, 0x18, 0x18, 0x18, 0x00, 0x10, 0x00, 0x00},
    {0x1F, 0x0F, 0x07, 0x03, 0x03, 0x03, 0x02, 0x03},
    {0x1F, 0x1F, 0x1F, 0x1F, 0x17, 0x03, 0x02, 0x03},
    {0x1F, 0x19, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};
uint8_t cactus[8] = {0x18, 0x18, 0x1B, 0x1B, 0x1B, 0x1F, 0x06, 0x06};

int jump_sound[][2] = {{350, 50}};
int game_over_sound[][2] = {{40, 100}, {0, 100}, {40, 100}};

int score = 0;
int cacti[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int offset = 3;
int last_offset = 0;
int jump_stage = -1;

void drawDinosaur(int x, int y) {
  for (int y = last_offset - 1; y <= last_offset; y++) {
    lcd.setCursor(DEFAULT_X, y);
    
    for (int x = DEFAULT_X; x <= DEFAULT_X + 3; x++) {
      lcd.print(" ");
    }
  }

  for (int i = 1; i <= 7; i++) {
    lcd.setCursor(x + i - (i <= 4 ? 1 : 5), y + (i <= 4 ? -1 : 0));
    lcd.write(i);
  }
}

void setup() {
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 8; i++) {
    lcd.createChar(i + 1, dinosaur[i]);
  }
  
  lcd.createChar(0, cactus);

  for (int i = 0; i <= DEFAULT_X; i++) {
    drawDinosaur(i, offset);
    delay(400);
    lcd.clear();
  }
}

void loop() {
  if (jump_stage != -1) {
    offset = offset + (jump_stage > 4 ? -1 : jump_stage >= 2 && jump_stage <= 4 ? 0 : 1);
    jump_stage--;
  } else if (!digitalRead(PIN_BUTTON)) {
    jump_stage = 6;

    for (auto i : jump_sound) {
      tone(PIN_BUZZER, i[0], i[1]);
      delay(i[1]);
    }
  }
  
  if (last_offset != offset) {
    drawDinosaur(DEFAULT_X, offset);
  }

  if (random(0, 4) == 0) {
    bool spawn = true;
    for (int i = 19; i >= random(10, 16); i--) {
      spawn = spawn ? cacti[i] == 0 : spawn;
    }
    cacti[19] = spawn;      
  }

  for (int i = 0; i < 20; i++) {
    if (cacti[i] == 1) {
      if (i >= DEFAULT_X && i < DEFAULT_X + 3 && offset >= 2) {
          lcd.setCursor(i + 1, 3);
          lcd.write(0);

          analogWrite(PIN_LED_RED, 255);
          lcd.setCursor(8, 1);
          lcd.print("Game");
          lcd.setCursor(8, 2);
          lcd.print("Over!");

          for (auto i : game_over_sound) {
            tone(PIN_BUZZER, i[0], i[1]);
            delay(i[1]);
          }

          delay(4000);

          analogWrite(PIN_LED_RED, 0);
          lcd.clear();
          reset();
      } 

      if (i != 19) {
        lcd.setCursor(i + 1, 3);
        lcd.print(" ");
      }
      
      if (i != 0) {
        lcd.setCursor(i, 3);
        lcd.write(0);
        cacti[i - 1] = 1;
      }
      cacti[i] = 0;
    }
  }

  for (int i = 0; i <= 5; i++) {
    if (score < pow(10, i + 1)) {
      lcd.setCursor(15, 0);

      for (int j = 4 - i; j != 0; j--) {
        lcd.print(0);
      }

      lcd.print(score);
      break;
    }
  }
  
  delay(1000 / FPS);
  last_offset = offset;
  score++;
}
