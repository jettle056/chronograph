#include <Wire.h>
#include <EEPROM.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;   // LCD object using I2C

#define firstsensor_1 10    // First IR sensor
#define secondsensor_2 11   // Second IR sensor
#define UP A1               // Increase pellet weight
#define DOWN A0             // Decrease pellet weight

float fps = 0;     
float ftlbs = 0;  
float pWeight;    
float elap, ms, joule;

float fpstotal = 0;
float average = 0;
float ftlbstotal = 0;
float energy = 0;

const int LCD_COLS = 20;
const int LCD_ROWS = 4;

int weightTens;
int weightDec;
int one = 1;              // Makes setup run once
int counter_shots = 0;

unsigned long time1;
unsigned long time2;

void setup() {

  lcd.begin(LCD_COLS, LCD_ROWS);

  // Load pellet weight from EEPROM
  weightTens = EEPROM.read(0);
  weightDec = EEPROM.read(1);
  pWeight = weightTens + float(weightDec) / 100;

  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);

  pinMode(firstsensor_1, INPUT);
  pinMode(secondsensor_2, INPUT);
  digitalWrite(firstsensor_1, HIGH);
  digitalWrite(secondsensor_2, HIGH);

  reset_variables();

  lcd.setCursor(3, 0);
  lcd.print("Chronograph V8.3");
  delay(2000);
}

void loop()
{
  if (one == 1)
    firstTime();

  everyTime();
}

void firstTime()
{
  one = 2;

  lcd.setCursor(1, 0);
  lcd.print("Enter pellet weight");
  lcd.setCursor(4, 2);
  lcd.print(pWeight);
  lcd.setCursor(9, 2);
  lcd.print("grains");
  delay(2000);

  while (digitalRead(UP) == LOW)
  {
    pWeight += 0.01;
    lcd.setCursor(4, 2);
    lcd.print(pWeight);
    weightDec = (pWeight - int(pWeight)) * 100;
    EEPROM.write(0, int(pWeight));
    EEPROM.write(1, weightDec);
    delay(300);
  }

  while (digitalRead(DOWN) == LOW)
  {
    pWeight -= 0.01;
    lcd.setCursor(4, 2);
    lcd.print(pWeight);
    weightDec = (pWeight - int(pWeight)) * 100;
    EEPROM.write(0, int(pWeight));
    EEPROM.write(1, weightDec);
    delay(300);
  }
}

void everyTime()
{
  lcd.setCursor(3, 3);
  lcd.print("Awaiting shot");
  lcd.blink();

  while (digitalRead(firstsensor_1) == 0);
  while (digitalRead(firstsensor_1));
  time1 = micros();

  while (digitalRead(secondsensor_2));
  time2 = micros();

  printserial();
}

void printserial()
{
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("Shot number: ");
  lcd.print(++counter_shots);

  elap = time2 - time1;

  fps = 196850 / elap;     // 60mm spacing
  lcd.setCursor(0, 1);
  lcd.print(fps);
  lcd.setCursor(6, 1);
  lcd.print("fps");

  ms = fps * 0.3048;
  lcd.setCursor(11, 1);
  lcd.print(ms);
  lcd.setCursor(17, 1);
  lcd.print("mps");

  ftlbs = (pWeight * fps * fps) / 450240;
  joule = ftlbs * 1.35582;

  lcd.setCursor(1, 2);
  lcd.print("Ft/Lbs:");
  lcd.print(ftlbs);

  lcd.setCursor(1, 3);
  lcd.print("Joules:");
  lcd.print(joule);

  delay(5000);
  lcd.clear();

  fpstotal += fps;
  average = fpstotal / counter_shots;
  ftlbstotal += ftlbs;
  energy = ftlbstotal / counter_shots;

  if (counter_shots == 5 || counter_shots == 10)
  {
    lcd.setCursor(4, 0);
    lcd.print("Shot Average");

    lcd.setCursor(0, 1);
    lcd.print("Velocity:");
    lcd.print(average);
    lcd.setCursor(17, 1);
    lcd.print("FPS");

    lcd.setCursor(0, 2);
    lcd.print("Energy:");
    lcd.print(energy);
    lcd.setCursor(14, 2);
    lcd.print("Ft/Lbs");
  }
}

void reset_variables()
{
  time1 = 0;
  time2 = 0;
}