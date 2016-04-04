//
//  Nixie Clock.ino
//
//  Created by Mr. Gecko's Media (James Coleman) on 7/23/14.
//  No Copyright Claimed. Public Domain.
//

#include <Wire.h>
#include "RTClib.h"
#include "OneWire.h"

#include "Nixies.h"
#include "Registers.h"

const unsigned int button1Pin = 10;
const unsigned int button2Pin = 11;
unsigned int button1State = 0;
unsigned int button2State = 0;

OneWire ds(2); // Temperature Sensor on pin 2
RTC_DS1307 RTC;

unsigned int serialPin = 5;
unsigned int serialClockPin = 7;
unsigned int registerClockPin = 6;

Nixies nixies;

void setup() {
	Serial.begin(9600);
	Wire.begin();
	
	RTC.begin();
	if (!RTC.isrunning()) {
		Serial.println("RTC needs to be set");
		RTC.adjust(DateTime(__DATE__, __TIME__));
	}
	
	pinMode(button1Pin, INPUT);
	pinMode(button2Pin, INPUT);       
	
	nixies.initialize(serialPin, serialClockPin, registerClockPin, 6);
}

void parseSerialData(char *data) {
	char *originalData = data;
	char *array[10];
	int arrayCount = 0;
	
	char *parse = strtok(data, ",");
	while (parse!=NULL) {
		array[arrayCount] = parse;
		arrayCount++;
		if (arrayCount>=9)
			return;
		parse = strtok(NULL, ",");
	}
	array[arrayCount] = NULL;
	
	if (arrayCount==0)
		return;
	if (strcmp(array[0],"pi")==0) {//Ping
		Serial.println(originalData);
	} else if (strcmp(array[0],"st")==0 && arrayCount==8) {// Set Time
		delay(atoi(array[7]));
		RTC.adjust(DateTime(atoi(array[1])/*Year*/, atoi(array[2])/*Month*/, atoi(array[3])/*Day*/, atoi(array[4])/*Hour*/, atoi(array[5])/*Minute*/, atoi(array[6])/*Second*/));
	} else if (strcmp(array[0],"dp")==0 && arrayCount==3) { // Display for MS.
		char *display = array[1];
		int length = strlen(display);
		if (length>6)
			return;
		for (int i=0; i<6; i++) {
			int value;
			if (i<length) {
				char digit = display[i];
				if (digit==' ') {
					value = 10;
				} else {
					value = atoi(&digit);
				}
			} else {
				value = 10;
			}
			nixies.setNixie(5-i, value);
		}
		nixies.update();
		delay(atoi(array[2]));
	} else if (strcmp(array[0],"test")==0) { // Run tests
		testTubes();
	} else if (strcmp(array[0],"temp")==0) { // Display Temperature
		displayTemperature();
	} else if (strcmp(array[0],"readtemp")==0) { // Display Temperature
		float temperature = getTemperatureFahrenheit();
		Serial.println(temperature);
	} else if (strcmp(array[0],"readtempc")==0) { // Display Temperature
		float temperature = getTemperature();
		Serial.println(temperature);
	}
}

void processSerialData() {
	unsigned long startRead = millis();
	char *bytesRead = (char *)malloc(50);
	int bytesLength = 0;
	bool goodRead = true;
	while (1) {
		if (Serial.available()) {
			char byteRead = Serial.read();
			// If we receive a end line, we received the response.
			if (byteRead=='\r')
				continue;
			if (byteRead=='\n')
				break;
			bytesRead[bytesLength] = byteRead;
			bytesLength++;
			// We don't want a buffer overrun. The responses from the bluetooth board is usually 3 characters long.
			if (bytesLength>=49) {
				goodRead = false;
				break;
			}
		}
		// Insure there isn't a error in reading and this doesn't continue reading infinite.
		if ((millis()-startRead)>=1000) {
			goodRead = false;
			break;
		}
	}
	bytesRead[bytesLength] = '\0';
	if (goodRead) {
		parseSerialData(bytesRead);
	}
	free(bytesRead);
}

void displayTemperature() {
	int temperature = getTemperatureFahrenheit();
	
	nixies.setNixie(5, temperature/10);
	nixies.setNixie(4, temperature%10);
	nixies.setNixie(3, 10);
	nixies.setNixie(2, 10);
	nixies.setNixie(1, 10);
	nixies.setNixie(0, 10);
	nixies.update();
	delay(2000);
}

void testTubes() {
	// Test every digit
	for (int i=0; i<30; i++) {
		nixies.setNixie(0, i%10);
		nixies.setNixie(1, (i+1)%10);
		nixies.setNixie(2, (i+2)%10);
		nixies.setNixie(3, (i+3)%10);
		nixies.setNixie(4, (i+4)%10);
		nixies.setNixie(5, (i+5)%10);
		nixies.update();
		delay(500);
	}
	// Test setting to blank
	nixies.setNixie(0, 10);
	nixies.setNixie(1, 10);
	nixies.setNixie(2, 10);
	nixies.setNixie(3, 10);
	nixies.setNixie(4, 10);
	nixies.setNixie(5, 10);
	nixies.update();
	delay(1000);
}

void displayTime() {
	DateTime now = RTC.now();
	int hour = now.hour();
	if (hour>12)
		hour -= 12;
	if (hour==0)
		hour = 12;
	int minute = now.minute();
	int second = now.second();
	
	nixies.setNixie(5, hour/10);
	nixies.setNixie(4, hour%10);
	nixies.setNixie(3, minute/10);
	nixies.setNixie(2, minute%10);
	nixies.setNixie(1, second/10);
	nixies.setNixie(0, second%10);
	nixies.update();
}

void loop() {
	if (Serial.available()) {
		processSerialData();
	}
	
	int state1 = digitalRead(button1Pin);
	if (state1!=button1State && state1==HIGH) {
		//displayTemperature();
		Serial.println("b1"); // Send button press to serial.
		button1State = state1;
	} else if (state1!=button1State && state1==LOW) {
		button1State = state1;
	}
	int state2 = digitalRead(button2Pin);
	if (state2!=button2State && state2==HIGH) {
		testTubes();
		Serial.println("b2"); // Send button press to serial.
		button2State = state2;
	} else if (state2!=button2State && state2==LOW) {
		button2State = state2;
	}
	
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
		Serial.println("CRC is not valid!");
		return -1000;
	}

	if (addr[0]!=0x10 && addr[0]!=0x28) {
		Serial.print("Device is not recognized");
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

float getTemperatureFahrenheit() {
	float temperature = getTemperature();
	return (temperature*(9.0/5.0))+32;
}
