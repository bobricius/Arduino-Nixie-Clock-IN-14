//
//  Registers.h
//
//  Created by Mr. Gecko's Media (James Coleman) on 7/23/14.
//  No Copyright Claimed. Public Domain.
//

#ifndef Registers_h
#define Registers_h

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Registers {
public:
	Registers();
	bool initialize(unsigned int theSerialPin, unsigned int theSerialClockPin, unsigned int theRegisterClockPin, unsigned int theRegisterCount);
	bool setPin(unsigned int pin, bool value);
	void setAllPins(bool value);
	void update();
private:
	unsigned int serialPin; // Also known as SER, or data pin
	unsigned int serialClockPin; // Also known as SRCLK or shift pin
	unsigned int registerClockPin; // Also known as RCLK, latch, or load pin
	unsigned int registerCount; // Number of registers in chian
	byte registerValues[25]; // 8-bit values for each register
};

#endif
