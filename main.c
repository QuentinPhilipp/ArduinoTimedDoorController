/*

*/
#include "data.h"
#include <DS3231.h>
#include <Wire.h>

#define DOWNMOTORPIN 2
#define UPMOTORPIN 3


unsigned long timer1Delay=1000;

DS3231 clock;
bool h12Flag=false;
bool pmFlag=false;
byte alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits;
bool alarmDy, alarmH12Flag, alarmPmFlag;

bool setupHour=false;
bool century=false;

byte closingMinute;
byte closingHour;

byte openingMinute;
byte openingHour;


bool opening=false;
unsigned long openingTime=2000;
unsigned long startOpening=0;

bool closing=false;
unsigned long closingTime=2000;
unsigned long startClosing=0;



void readData(){
  // Serial.println("Read Data");

  // Opening time
  String openTime = calendarOn[clock.getMonth(century)-1];
  Serial.print("Opening time :");
  Serial.println(openTime);
  openingHour = (byte)openTime.substring(0,2).toInt();
  openingMinute = (byte)openTime.substring(3,5).toInt();

  // Closing time
  String closeTime = calendarOff[clock.getMonth(century)-1];
  Serial.print("Closing time :");
  Serial.println(closeTime);
  closingHour = (byte)closeTime.substring(0,2).toInt();
  closingMinute = (byte)closeTime.substring(3,5).toInt();
}


void openDoor(){
  Serial.println("Open Door");
  startOpening=millis();
  opening=true;
}
void closeDoor(){
  Serial.println("Close Door");
  startClosing=millis();
  closing=true;
}



void setAlarms() {
  readData();

  byte day=1;
  byte hour=0;
  byte minute=0;
  byte second=0;
  byte bits=0x0;

  clock.setA1Time(day,openingHour,openingMinute,second,bits,false,false,false);
  clock.setA2Time(day,closingHour,closingMinute,bits,false,false,false);

  // Turn on both alarms, with external interrupt
	clock.turnOnAlarm(1);
	clock.turnOnAlarm(2);

}



void setup() {
	// Start the I2C interface
	Wire.begin();
	// Start the serial interface
	Serial.begin(57600);


  if (setupHour==true) {
    clock.setYear(21);
    clock.setMonth(1);
    clock.setDate(18);
    clock.setDoW(0);
    clock.setHour(16);
    clock.setMinute(15);
    clock.setSecond(30);
  }
  setAlarms();

}


// Called every second
void loopSecond() {

  byte hour = clock.getHour(h12Flag, pmFlag);
  byte minutes = clock.getMinute();
  byte seconds = clock.getSecond();

  byte day = clock.getDate();
  byte month = clock.getMonth(century);

  // Set new alarms at midnight
  if(hour==0 && minutes==0 && seconds<5) {
    setAlarms();
  }

	// Finally the hour, minute, and second
  Serial.print(day, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print(" ");
	Serial.print(hour, DEC);
	Serial.print("h");
	Serial.print(minutes, DEC);
	Serial.print(":");
	Serial.print(seconds, DEC);

	// Check alarms
	if (clock.checkIfAlarm(1)) {
    openDoor();
	}

	if (clock.checkIfAlarm(2)) {
    closeDoor();
	}

	// New line on display
	Serial.println();

	// Display Alarm 1 information
	Serial.print("Alarm 1: ");
	clock.getA1Time(alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits, alarmDy, alarmH12Flag, alarmPmFlag);
	Serial.print(alarmDay, DEC);
  Serial.print(" Day");
	Serial.print(' ');
	Serial.print(alarmHour, DEC);
	Serial.print('h');
	Serial.print(alarmMinute, DEC);
  Serial.println();

	// Display Alarm 2 information
	Serial.print("Alarm 2: ");
	clock.getA2Time(alarmDay, alarmHour, alarmMinute, alarmBits, alarmDy, alarmH12Flag, alarmPmFlag);
	Serial.print(alarmDay, DEC);
  Serial.print(" Day");
	Serial.print(' ');
	Serial.print(alarmHour, DEC);
	Serial.print('h');
	Serial.print(alarmMinute, DEC);
  Serial.println();
	Serial.println("\n");
}


unsigned long timer1=0;

void loop() {

  // Check motor state
  if ((millis()-startOpening)>openingTime) {
    opening=false;
  }
  if ((millis()-startOpening)>closingTime) {
    closing=false;
  }

  // Move motor according to motor state
  // Higher priority for closing, security purpose
  if (closing==true) {
    digitalWrite(UPMOTORPIN,LOW);
    digitalWrite(DOWNMOTORPIN,HIGH);
    Serial.println("Closing");
  }
  else if (opening==true) {
      digitalWrite(DOWNMOTORPIN,LOW);
      digitalWrite(UPMOTORPIN,HIGH);
      Serial.println("Opening");
  }
  else {
    digitalWrite(DOWNMOTORPIN,LOW);
    digitalWrite(UPMOTORPIN,LOW);
  }




  if (timer1>1000){
    timer1=0;
    loopSecond();
  }
  timer1+=10;

	delay(10);
}
