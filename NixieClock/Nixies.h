//
//  Nixies.h
//
//  Created by Mr. Gecko's Media (James Coleman) on 7/23/14.
//  No Copyright Claimed. Public Domain.
//

#ifndef Nixies_h
#define Nixies_h

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Registers.h"

class Nixies {
public:
	Nixies();
	bool initialize(unsigned int theSerialPin, unsigned int theSerialClockPin, unsigned int theRegisterClockPin, unsigned int theNixieCount);
	bool setNixie(unsigned int nixieNumber, unsigned int value); // 10 = off
	void setAllNixies(unsigned int value);
	void update();
private:
	Registers nixieRegisters;
	unsigned int nixieCount; // Number of nixies
	unsigned int nixieValues[50];
	
	void setPinsWithValue(int startingPin, int value);
};

#endif
