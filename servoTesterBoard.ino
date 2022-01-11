#include <U8g2lib.h>
#include <Servo.h>


//u8g2LIB_SSD1306_128X64 u8g2(u8g2_I2C_OPT_NONE|u8g2_I2C_OPT_DEV_0);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

int xmax = 128;
int ymax = 62;
int xcenter = xmax / 2;
int ycenter = ymax / 2 + 10;
int arc = ymax / 2;
int angle = 0;

char* label[] = {"POSITION"};
int labelXpos[] = {45};

int potVal, mappedAngle, mappledNeedleLocation, a = 10;
u8g2_uint_t mappedNeedleAngle = 0;

int servoPin = 5;  // analog pin used to connect the potentiometer
Servo myservo;  // create servo object to control a servo
int targetSetAngle = 90;  // set angle when not using pot

#define potmeterPin A1
int switchPos1Pin = 8;  // up
int switchPos2Pin = 7;  // down
int button1Pin = 6;  // down
int button1State = 0;         // variable for reading the pushbutton status

int servoVoltagePin = A1; //
int servoVoltageVal = 1;
float servoVoltageActual = 1.000;

int controlMode = 1;  // up

int minPotValue = 0;  // for calibration
int maxPotValue = 890;  //for calibration. 1023 originally


//FUNCTIONS ------------------------------------------------------------------------------------
void gauge(uint8_t angle) {

  // draw border of the gauge
  u8g2.drawCircle(xcenter, ycenter, arc + 6, U8G2_DRAW_UPPER_RIGHT);
  u8g2.drawCircle(xcenter, ycenter, arc + 4, U8G2_DRAW_UPPER_RIGHT);

  u8g2.drawCircle(xcenter, ycenter, arc + 6, U8G2_DRAW_UPPER_LEFT);
  u8g2.drawCircle(xcenter, ycenter, arc + 4, U8G2_DRAW_UPPER_LEFT);
  // draw the needle
  float x1 = sin(2 * angle * 2 * 3.14 / 360);
  float y1 = cos(2 * angle * 2 * 3.14 / 360);
  u8g2.drawLine(xcenter, ycenter, xcenter + arc * x1, ycenter - arc * y1);
  u8g2.drawDisc(xcenter, ycenter, 5, U8G2_DRAW_UPPER_LEFT);
  u8g2.drawDisc(xcenter, ycenter, 5, U8G2_DRAW_UPPER_RIGHT);
  u8g2.setFont(u8g2_font_chikita_tr);
  // show scale labels
  u8g2.drawStr( 20, 42, "0");
  u8g2.drawStr( 25, 18, "45");
  u8g2.drawStr( 60, 14, "90");
  u8g2.drawStr( 95, 18, "135");
  u8g2.drawStr( 105, 42, "180");

  // label for swicth
  u8g2.setCursor(20, 60);
  u8g2.print(controlMode);

  // show gauge label

  u8g2.setCursor(labelXpos[0], 32);
  u8g2.print(label[0]);
  // show digital value and align its position
  u8g2.setFont(u8g2_font_profont22_tr);

  u8g2.setCursor(54, 60);

  /* //in original code, fix for low values? maybe have this fix upper value?
    if (mappedAngle < 10) {
    u8g2.print("0");
    }
  */

  //shift over when values are greater than 100; to be pretty
  if (mappedAngle > 99) {
    //u8g.setPrintPos(47,60);
    u8g2.setCursor(47, 60);
  }
  u8g2.print(mappedAngle);
  u8g2.setFont(u8g2_font_chikita_tr);

  //prints out voltage VALUE to OLED
  u8g2.setCursor(115, 5);
  u8g2.print(servoVoltageVal);

  //prints out voltage to OLED
  u8g2.setCursor(95, 60);
  u8g2.print(servoVoltageActual);
  u8g2.drawStr( 120, 60, "v");
}

//SETUP ------------------------------------------------------------------------------------
void setup(void) {

  Serial.begin(115200);

  //servo
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object

  //switch
  pinMode(switchPos1Pin, INPUT_PULLUP);
  pinMode(switchPos2Pin, INPUT_PULLUP);
  pinMode(button1Pin, INPUT_PULLUP);

  //screen
  u8g2.begin();
  u8g2.setFont(u8g2_font_chikita_tr);
  Serial.print("Setup done");
}

//LOOP ------------------------------------------------------------------------------------
void loop(void) {


  //voltage read and print
  servoVoltageVal = analogRead(servoVoltagePin);

  //if it reads nothing so USB POWERED
  if (digitalRead(servoVoltageVal) <= 100) { //100 just arbitrary - really reads 0-3
    servoVoltageActual = 5.00;

  }
  else {
    servoVoltageActual = servoVoltageVal / 105.5;
  }

  //servoVoltageActual = servoVoltageVal * (5.0 / 1023.0);
  //servoVoltageActual = servoVoltageActual * 2.5; //voltage divider adjustment

  //Serial.print("Read: ");
  //Serial.println(servoVoltageVal);
  //Serial.print("converted voltage: ");
  //Serial.println(servoVoltageActual);


  //SWITCH
  if (digitalRead(switchPos1Pin) == HIGH) {
    controlMode = 1;
    u8g2.setCursor(20, 60);
    u8g2.print(controlMode);
  }
  if (digitalRead(switchPos1Pin) == LOW) {
    controlMode = 2;
    u8g2.setCursor(20, 60);
    u8g2.print(controlMode);
  }
  //Serial.println(controlMode);
  if (controlMode == 1) {
    //POT
    //Serial.println("mode 1");
    potVal = analogRead(A0);         //p
  }
  else if (controlMode == 2) {

    button1State = digitalRead(button1Pin);
    if (button1State == LOW) {

      if (targetSetAngle >= 181) {
        targetSetAngle = 0;
      }
      else {
        targetSetAngle = targetSetAngle + 45;
      }
    }
    //POT
    Serial.println("targetSetAngle");

    potVal = map(targetSetAngle, 0, 180, minPotValue, maxPotValue); //w

    //mappedAngle = map(potVal, minPotValue, maxPotValue, 0, 180); //w

  }

  //mappedAngle = map(potVal, minPotValue, maxPotValue, 0, 180); //w
  //mappledNeedleLocation = map(potVal, minPotValue, maxPotValue, 0, 90);    //m
  Serial.println(potVal);
  mappedAngle = map(potVal, minPotValue, maxPotValue, 0, 180); //w
  mappledNeedleLocation = map(potVal, minPotValue, maxPotValue, 0, 90);    //m

  // show needle and dial
  mappedNeedleAngle = mappledNeedleLocation; //xx
  if (mappedNeedleAngle < 45) {
    mappedNeedleAngle = mappedNeedleAngle + 135;
  }
  else {
    mappedNeedleAngle = mappedNeedleAngle - 45;
  }

  // picture loop
  {
    u8g2.firstPage();
    do {
      gauge(mappedNeedleAngle);
    }
    while ( u8g2.nextPage() );
  }

  // move servo

  myservo.write(mappedAngle);

}
