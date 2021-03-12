//#include <Arduino.h> // For PlatformIO
#include <M5StickC.h>  // https://github.com/m5stack/M5StickC 
#include <BleMouse.h>  // https://github.com/T-vK/ESP32-BLE-Mouse

#include <MiniTrackball.h>  //Included library to interface with GROVE Trackball Module
#include <Wire.h>  //Used by MiniTrackball

BleMouse bleMouse;
MiniTrackBall miniTrackball;

float accX              = 0;
float accY              = 0;
float accZ              = 0;

int     puffer          = 0;
int     x               = 0;
int     y               = 0;
int     z               = 0;

int sensitivity = 1;

boolean isActivated     = true;
boolean buttonIsPressed = false;
//boolean tipped          = false;
boolean led             = false;

String  lcdText         = " ";

byte counter            = 0;

void writeText(String _Text, int _ColorText) {
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.println(lcdText);
  M5.Lcd.setTextColor(_ColorText);
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.println(_Text);
  lcdText = _Text;
}

void printXY (int _ColorText) {
  M5.Lcd.setTextColor(_ColorText);
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.printf("X Input: %d   ", x);
  M5.Lcd.setCursor(0, 120);
  M5.Lcd.printf("Y Input: %d   ", y);
  M5.Lcd.setCursor(0, 140);
  M5.Lcd.printf("Sensitivity: %d   ", sensitivity);
}

void setup() {
  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, HIGH);
  bleMouse.begin();
  M5.begin();
  M5.Lcd.setRotation(2);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  writeText("Trackball ON", WHITE);
  //Initialize Gyro
  //M5.MPU6886.Init();
  //M5.MPU6886.SetGyroFsr(M5.MPU6886.GFS_250DPS);
  //M5.MPU6886.SetAccelFsr(M5.MPU6886.AFS_2G);

  //Initialize Trackball
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  //mini-trackball initialization
  uint8_t track_data[5];
  miniTrackball.GetTrackData(track_data);
  //.....
  
  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    if (!isActivated) {
      isActivated = true;
      writeText("Trackball ON", WHITE);
    } else {
      isActivated = false;
      writeText("Trackball OFF", RED);
      bleMouse.release(MOUSE_MIDDLE);
    }
    while (digitalRead(M5_BUTTON_HOME) == LOW) {
    }
  }
  
  //Translate Motion to Mouse Input
  if (bleMouse.isConnected() && isActivated) {
    printXY(BLACK);
    //M5.MPU6886.getAccelData(&accX, &accY, &accZ);
    
    //determine how much mouse should move
    //x = accX * 15;
    //y = accY * 15;

    
    Serial.print("MOTION_X:  ");
    x = (track_data[3] - track_data[2])*sensitivity;
    Serial.println(x, DEC);
    Serial.print("MOTION_Y:  ");
    y = (track_data[0] - track_data[1])*sensitivity;
    Serial.println(y, DEC);
    Serial.print("MOTION_CLICK: ");
    Serial.println(track_data[4], DEC);
    Serial.println("+------------------------------------------------------+");
    Serial.println(" ");

  /*Comment out Z Azis RE: acceleromerter motion
    //delay(1000);
    //check whether ther eis a Z-axiz change > 1.1
    if ( accZ * 100 > 110)
      z += 1;
    else {
      if (z > 1)
        z--;
    }
    //Register 'tipped' status for large change in Z
    if (z > 25 && !tipped) {
      tipped = true;

      if (led) {
        digitalWrite(M5_LED, LOW);
        led = false;
      }
      else {
        digitalWrite(M5_LED, HIGH);
        led = true;
      }
    }
    //removed 'tipped' status when mouse is returned to level
    if (z < 5 && tipped) {
      tipped = false;
    }
  */
    printXY(WHITE);
    if (abs(x) + abs(y) == 0 && buttonIsPressed) {
      if (puffer < 1) {
        writeText("Trackball Active", BLUE);
        bleMouse.release(MOUSE_MIDDLE);
        buttonIsPressed = false;
      } else
        puffer--;
    }
    if (abs(x) > 0 ||  abs(y) > 0 ) {
      writeText("Trackball Active", GREEN);
      puffer = 100;
      if (!buttonIsPressed && !led) {
        bleMouse.press(MOUSE_MIDDLE);
        buttonIsPressed = true;
      }
      bleMouse.move(x, -y);
    }
    delay(2);
  }
}
