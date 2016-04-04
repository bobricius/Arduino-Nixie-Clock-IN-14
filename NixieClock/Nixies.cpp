//
//  nixies.cpp
//
//  Created by Mr. Gecko's Media (James Coleman) on 7/23/14.
//  No Copyright Claimed. Public Domain.
//

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Nixies.h"

Nixies::Nixies() {
	nixieCount = 0;
}

bool Nixies::initialize(unsigned int theSerialPin, unsigned int theSerialClockPin, unsigned int theRegisterClockPin, unsigned int theNixieCount) {
	if (theNixieCount>50)
		return false;
	
	unsigned int registerCount = (theNixieCount/2)+(theNixieCount%2);
	if (!nixieRegisters.initialize(theSerialPin, theSerialClockPin, theRegisterClockPin, registerCount))
		return false;
	
	nixieCount = theNixieCount;
	
	setAllNixies(0);
	return true;
}

bool Nixies::setNixie(unsigned int nixieNumber, unsigned int value) {
	if (nixieNumber>=nixieCount)
		return false;
	if (value>10)
		value = 10;
	
	nixieValues[nixieNumber] = value;
	return true;
}

void Nixies::setAllNixies(unsigned int value) {
	if (nixieCount==0)
		return;
	if (value>10)
		value = 10;
	for (int nixieNumber=0; nixieNumber<nixieCount; nixieNumber++) {
		setNixie(nixieNumber, value);
	}
}

static const byte pinValues[11] = {0x0, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0xA};
void Nixies::setPinsWithValue(int startingPin, int value) {
	if (value>10)
		value = 10;
	byte values = pinValues[value];
	for (int pinOffset=0; pinOffset<4; pinOffset++) {
		bool pinValue = values & (1 << pinOffset);
		nixieRegisters.setPin(startingPin+pinOffset, pinValue);
	}
}

void Nixies::update() {
	if (nixieCount==0)
		return;
	for (int nixieNumber=0; nixieNumber<nixieCount; nixieNumber++) {
		unsigned int nixieValue = nixieValues[nixieNumber];
		setPinsWithValue(nixieNumber*4, nixieValue);
	}
	nixieRegisters.update();
}
