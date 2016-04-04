//
//  Registers.cpp
//
//  Created by Mr. Gecko's Media (James Coleman) on 7/23/14.
//  No Copyright Claimed. Public Domain.
//

#include "Registers.h"

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Registers::Registers() {
	serialPin = 0;
	serialClockPin = 0;
	registerClockPin = 0;
	
	registerCount = 0;
}

bool Registers::initialize(unsigned int theSerialPin, unsigned int theSerialClockPin, unsigned int theRegisterClockPin, unsigned int theRegisterCount) {
	if (theRegisterCount>25)
		return false;
	
	serialPin = theSerialPin;
	serialClockPin = theSerialClockPin;
	registerClockPin = theRegisterClockPin;
	
	pinMode(serialPin, OUTPUT);
	pinMode(serialClockPin, OUTPUT);
	pinMode(registerClockPin, OUTPUT);
	
	registerCount = theRegisterCount;
	
	setAllPins(LOW);
	return true;
}

bool Registers::setPin(unsigned int pin, bool value) {
	unsigned int registerNumber = pin/8;
	unsigned int pinNumber = pin%8;
	if (registerNumber>=registerCount)
		return false;
	
	byte registerValue = registerValues[registerNumber];
	registerValue &= ~(1 << pinNumber); // Sets pin to LOW
	registerValue |= (value << pinNumber); // Sets pin to value
	registerValues[registerNumber] = registerValue;
	return true;
}

void Registers::setAllPins(bool value) {
	if (registerCount==0)
		return;
	for (int i=0; i<(registerCount*8); i++) {
		setPin(i, value);
	}
}

void Registers::update() {
	if (registerCount==0)
		return;
	// Prepare for updating
	digitalWrite(registerClockPin, LOW);
	digitalWrite(serialPin, LOW);
	digitalWrite(serialClockPin, LOW);
	
	for (int registerNumber = registerCount-1; registerNumber>=0; registerNumber--) {
		byte registerValue = registerValues[registerNumber];
		for (int pinNumber = 7; pinNumber>=0; pinNumber--) {
			digitalWrite(serialClockPin, LOW); // Tell register that a value is getting ready to be written
			
			bool value = registerValue & (1 << pinNumber); // Grab value for pin
			digitalWrite(serialPin, value); // Write value
			
			digitalWrite(serialClockPin, HIGH); // Tell register to capture new value
		}
	}
	
	digitalWrite(serialClockPin, LOW); // Close out register
	digitalWrite(registerClockPin, HIGH); // Make new values live
}