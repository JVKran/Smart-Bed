#include <Arduino.h>
#include "LoadSensor.hpp"

LoadSensor::LoadSensor(uint8_t dataOutPin, uint8_t serialClockPin, uint8_t gain, const bool autoBegin):
	serialClockPin(serialClockPin),
	dataOutPin(dataOutPin)
{
	if(autoBegin){
		pinMode(serialClockPin, OUTPUT);
		pinMode(dataOutPin, INPUT);

		setGain(gain);
	}
}

void LoadSensor::begin(){
	pinMode(serialClockPin, OUTPUT);
	pinMode(dataOutPin, INPUT);

	setGain(gain);
}

bool LoadSensor::isReady() {
	return !digitalRead(dataOutPin);
}

void LoadSensor::setGain(uint8_t newGain) {
	switch (newGain) {
		case 128:
			gain = 1;
			break;
		case 64:
			gain = 3;
			break;
		default:
			gain = 2;
			break;
	}
}

long LoadSensor::read() {
	while (!isReady());

    unsigned long reading = 0;
    uint8_t data[3] = { 0 };
    uint8_t filler = 0x00;

	// pulse the clock pin 24 times to read the data
    data[2] = shiftIn(dataOutPin, serialClockPin, MSBFIRST);
    data[1] = shiftIn(dataOutPin, serialClockPin, MSBFIRST);
    data[0] = shiftIn(dataOutPin, serialClockPin, MSBFIRST);

	// Set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < gain; i++) {
		digitalWrite(serialClockPin, HIGH);
		digitalWrite(serialClockPin, LOW);
	}

    // Datasheet indicates the reading is returned as a two's complement reading
    // Flip all the bits
    data[2] = ~data[2];
    data[1] = ~data[1];
    data[0] = ~data[0];

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if ( data[2] & 0x80 ) {
        filler = 0xFF;
    } else if ((0x7F == data[2]) && (0xFF == data[1]) && (0xFF == data[0])) {
        filler = 0xFF;
    } else {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    reading = ( static_cast<unsigned long>(filler) << 24
            | static_cast<unsigned long>(data[2]) << 16
            | static_cast<unsigned long>(data[1]) << 8
            | static_cast<unsigned long>(data[0]) );

    return static_cast<long>(++reading);
}

long LoadSensor::readAverage(uint8_t amountOfReadings) {
	long sum = 0;
	for (uint8_t i = 0; i < amountOfReadings; i++) {
		sum += read();
	}
	return sum / amountOfReadings;
}

double LoadSensor::getValue(uint8_t amountOfReadings) {
	return readAverage(amountOfReadings) - offset;
}

float LoadSensor::getUnits(uint8_t amountOfReadings) {
	return getValue(amountOfReadings) / scale;
}

void LoadSensor::tare(uint8_t amountOfReadings) {
	double sum = readAverage(amountOfReadings);
	setOffset(sum);
}

void LoadSensor::setScale(float newScale) {
	scale = newScale;
}

float LoadSensor::getScale() {
	return scale;
}

void LoadSensor::setOffset(long newOffset) {
	offset = newOffset;
}

long LoadSensor::getOffset() {
	return offset;
}

void LoadSensor::powerDown() {
	digitalWrite(serialClockPin, LOW);
	digitalWrite(serialClockPin, HIGH);
}

void LoadSensor::powerUp() {
	digitalWrite(serialClockPin, LOW);
}
