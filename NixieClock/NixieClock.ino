#include <Wire.h>
#include "RTClib.h"
#include "OneWire.h"

#include "Nixies.h"
#include "Registers.h"


OneWire ds(4); // Temperature Sensor on pin 2
RTC_DS1307 RTC;

const int led1 =  9;      // the number of the LED pin
const int led2 =  10;      // the number of the LED pin

#define modeBtn   2
#define hoursBtn   14
#define minutesBtn   15

unsigned int serialPin = 7 ;
unsigned int serialClockPin = 6;
unsigned int registerClockPin = 5;
int hour, minute, second; 

Nixies nixies;

void setup() {
//	Serial.begin(9600);
	Wire.begin();
	
	RTC.begin();
	if (!RTC.isrunning()) {
//		Serial.println("RTC needs to be set");
		RTC.adjust(DateTime(__DATE__, __TIME__));
	}

 pinMode(led1, OUTPUT);
 pinMode(led2, OUTPUT);   
   
 pinMode(modeBtn, INPUT);
 pinMode(hoursBtn, INPUT);    
 pinMode(minutesBtn, INPUT);

 digitalWrite(modeBtn, HIGH);
 digitalWrite(hoursBtn, HIGH);    
 digitalWrite(minutesBtn, HIGH);
 

	nixies.initialize(serialPin, serialClockPin, registerClockPin, 6);
}

//		RTC.adjust(DateTime(atoi(array[1])/*Year*/, atoi(array[2])/*Month*/, atoi(array[3])/*Day*/, atoi(array[4])/*Hour*/, atoi(array[5])/*Minute*/, atoi(array[6])/*Second*/));
    // following line sets the RTC to the date & time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));





void displayTime() {

	nixies.setNixie(5, hour/10);
	nixies.setNixie(4, hour%10);
	nixies.setNixie(3, minute/10);
	nixies.setNixie(2, minute%10);
	nixies.setNixie(1, second/10);
	nixies.setNixie(0, second%10);
	nixies.update();
}

void loop() {
	  DateTime now = RTC.now();
   hour = now.hour();
   minute = now.minute();
   second = now.second();

if (second==0)
{
  testTubes();
}
  
  if (digitalRead(hoursBtn)==LOW)
  {
    hour++;
    if (hour > 23) {
      hour = 0;
    }
    if (hour < 0) {
      hour = 23;
    }
  
    digitalWrite(led2, HIGH);
    delay(110);
    digitalWrite(led2, LOW);
    delay(110); 
     second=1; 
    RTC.adjust(DateTime(2014, 1, 21, hour, minute, second));
  }

  if (digitalRead(minutesBtn)==LOW)
  {
     minute++;
    if (minute > 59) {
      minute = 0;
    }
    if (minute < 0) {
      minute = 59;
    }

    digitalWrite(led2, HIGH);
    delay(110);
    digitalWrite(led2, LOW);
    delay(110); 
     second=1; 
    RTC.adjust(DateTime(2014, 1, 21, hour, minute, second));
  }

    if (digitalRead(modeBtn)==LOW)
  {

    displayTemperature();
//    testTubes();
  }

//    digitalWrite(led1, HIGH);
//    delay(100);
//    digitalWrite(led1, LOW);
//    delay(100);


//
//
//  // fade in from min to max in increments of 5 points:
//  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
//    // sets the value (range from 0 to 255):
//    analogWrite(led2, fadeValue);
//    // wait for 30 milliseconds to see the dimming effect
//    delay(1);
//  }
//
//  // fade out from max to min in increments of 5 points:
//  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
//    // sets the value (range from 0 to 255):
//    analogWrite(led2, fadeValue);
//    // wait for 30 milliseconds to see the dimming effect
//    delay(1);
//  }
  
  displayTime();
}

// Code for this function from http://bildr.org/2011/07/ds18b20-arduino/
// Copyright (c) 2010 bildr community

float getTemperature() {
	//returns the temperature from one DS18S20 in DEG Celsius
	byte data[12];
	byte addr[8];
	
	if (!ds.search(addr)) {
		//no more sensors on chain, reset search
		ds.reset_search();
		return -1000;
	}
	
	if (OneWire::crc8(addr, 7)!=addr[7]) {
//		Serial.println("CRC is not valid!");
		return -1000;
	}

	if (addr[0]!=0x10 && addr[0]!=0x28) {
//		Serial.print("Device is not recognized");
		return -1000;
	}

	ds.reset();
	ds.select(addr);
	ds.write(0x44,1); // start conversion, with parasite power on at the end

	byte present = ds.reset();
	ds.select(addr);
	ds.write(0xBE); // Read Scratchpad


	for (int i=0; i<9; i++) { // we need 9 bytes
		data[i] = ds.read();
	}

	ds.reset_search();

	byte MSB = data[1];
	byte LSB = data[0];

	float tempRead = ((MSB << 8) | LSB); //using two's compliment
	float TemperatureSum = tempRead / 16;

	return TemperatureSum;
}


void displayTemperature() {
  int temperature = getTemperature();
  
  nixies.setNixie(5, temperature/10);
  nixies.setNixie(4, temperature%10);
  nixies.setNixie(3, 10);
  nixies.setNixie(2, 10);
  nixies.setNixie(1, 10);
  nixies.setNixie(0, 10);
  nixies.update();
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(led1, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(led1, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

void testTubes() {
  // Test every digit
  for (int i=0; i<10; i++) {
   DateTime now = RTC.now();
   second = now.second();
    
  nixies.setNixie(5, i);
  nixies.setNixie(4, i);
  nixies.setNixie(3, i);
  nixies.setNixie(2, i);
  nixies.setNixie(1, second/10);
  nixies.setNixie(0, second%10);
    nixies.update();
    digitalWrite(led2, HIGH);
    delay(1);
    digitalWrite(led2, LOW);
    delay(200);
  }

//  // Test setting to blank
//  nixies.setNixie(0, 10);
//  nixies.setNixie(1, 10);
//  nixies.setNixie(2, 10);
//  nixies.setNixie(3, 10);
//  nixies.setNixie(4, 10);
//  nixies.setNixie(5, 10);
//  nixies.update();
//
//    digitalWrite(led1, HIGH);
//    delay(300);
//    digitalWrite(led1, LOW);
}

