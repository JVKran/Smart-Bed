#ifndef __LOAD_SENSOR_HPP
#define __LOAD_SENSOR_HPP

class LoadSensor {
	private:
		uint8_t serialClockPin;
		uint8_t dataOutPin;

		uint8_t gain;
		long offset;
		float scale;
	public:
		LoadSensor(uint8_t dataOutPin, uint8_t serialClockPin, uint8_t gain = 128, const bool autoBegin = false);
		void begin();

		bool isReady();

		void tare(uint8_t amountOfReadings = 10);
		void setGain(uint8_t newGain = 128);

		long read();
		long readAverage(uint8_t amountOfReadings = 10);

		double getValue(uint8_t amountOfReadings = 1);
		float getUnits(uint8_t amountOfReadings = 1);

		void setScale(float newScale = 1.f);
		float getScale();

		void setOffset(long newOffset = 0);
		long getOffset();

		void powerDown();
		void powerUp();
};

#endif //__LOAD_SENSOR_HPP