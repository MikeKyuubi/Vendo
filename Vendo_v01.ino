#include <LiquidCrystal_I2C.h>
#include "Countimer.h"

Countimer timer;

const int buttonPin1 = 4, buttonPin2 = 3, buttonPin3 = 7;   
int coinPin = 2;
int button_State1 = 0, button_State2 = 0, button_State3 = 0;
int count_value = 0;
int prestate = 0;

bool pump = false;

int seconds = 0;
int time_s = 0;
int time_m = 0;
int time_h = 0;

// To count pulses
int i = 0;
int impulseCount = 0;
int prevImpulse = 1;

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  Serial.begin(9600);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  
  lcd.init();
  lcd.backlight();
  homeScreen();

  pinMode(3, INPUT);
  timer.setInterval(refreshClock, 1000);
}

void homeScreen(){
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(4,0);
  lcd.print("Vendo");
}

void refreshClock(){
  lcd.setCursor(0,0);
  lcd.print("Current time: ");
  lcd.setCursor(0,1);
  lcd.print("> ");
  lcd.print(timer.getCurrentTime());
  seconds--;
}

void onComplete() {
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("= ");
  lcd.print("00:00:00");
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Thank you!");
  pump = false;
  delay(1500);
  homeScreen();
}

void coinInserted(){
    seconds = seconds + 13;

    time_m = seconds / 60;
    time_h = time_m / 60;
    time_s = seconds % 60;
    
    timer.setCounter(time_h, time_m, time_s, timer.COUNT_DOWN, onComplete);
    prestate = 1;
    lcd.setCursor(0,0);
    lcd.print("Coin Inserted");
    lcd.setCursor(0,1);
    lcd.print("= ");
    lcd.print(timer.getCurrentTime());
}

void loop() {
  i++; // Delay for each incoming coin
  if (i <= 45){
    if (digitalRead(coinPin) == 0 and prevImpulse == 1){
      prevImpulse = 0;
      impulseCount++;
    }
  }
  else{
    impulseCount = 0;
  }
  if (impulseCount == 5){
    coinInserted();
    impulseCount = 0;
  }

  if (digitalRead(coinPin) == 0 && prevImpulse == 1 && i > 45){
    i = 0;
    prevImpulse = 0;
    impulseCount++;
  } else {
    prevImpulse = digitalRead(coinPin);
  }

  Serial.print("i=");
  Serial.print(i);
  Serial.print(" Impulses:");
  Serial.print(impulseCount);
  Serial.print(" Prev:");
  Serial.print(prevImpulse);
  Serial.print(" Pin 3:");
  Serial.println(digitalRead(coinPin));
  
  // read the state of the pushbutton value:
  button_State1 = digitalRead(buttonPin1);
  button_State2 = digitalRead(buttonPin2);
  button_State3 = digitalRead(buttonPin3);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (button_State2 == HIGH && prestate == 0) {
    if (pump == false && time_s == 0 && time_m == 0 && time_h == 0){
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print("Insert coin");
      prestate = 1;
    }
    else{
      pump = true;
      prestate = 1;
    }
  }
  else if (button_State1 == HIGH && prestate == 0) { 
    pump = false;
    prestate = 1;
    lcd.setCursor(0,1);
    lcd.print("= ");
  }
  else if (button_State3 == HIGH && prestate == 0) { 
    pump = false;
    seconds = 0;

    time_m = seconds / 60;
    time_h = time_m / 60;
    time_s = seconds % 60;
    timer.setCounter(time_h, time_m, time_s, timer.COUNT_DOWN, onComplete);
    onComplete();
  }
  else if(button_State1 == LOW && button_State2 == LOW && button_State3 == LOW && prestate == 1) {
    prestate = 0;
  }

  time_s = timer.getCurrentSeconds();

  if (pump == true){
    if (time_s > 0 || time_m > 0 || time_h > 0){
          timer.run();
      if(!timer.isCounterCompleted()) {
        timer.start();
      }
    }
    else if (time_s == 0 && time_m == 0 && time_h == 0){
      onComplete();
    }
  }
}
