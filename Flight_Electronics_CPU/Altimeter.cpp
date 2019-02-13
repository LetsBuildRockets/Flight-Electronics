/*
 * Altimeter.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: erics
 */

#include "Altimeter.h"

void Altimeter::init()
{
	Wire.setSCL(33);
	Wire.setSDA(34);
	Wire.begin(); // this might be the second time we call wire.begin... we need to make sure it doesn't cause problems
	Wire.setClock(I2C_CLOCK_SPEED);
	Wire.setTimeout(I2C_TIMEOUT);

	velocity = 0;
	pressure = 0;

	send16BitWord(0xEFC8); // ask for part number
	uint8_t buf[2] = {0};
	readBytes(buf, 2);
	if(buf[1]&0b1000)
	{
		uint8_t partNumber = buf[1] & 0b11111; // we don't need the msb... ((buf[0] << 8) | buf[1]) & 0b11111;

		char buffer[5];
		itoa(partNumber & 0b11111, buffer, 2);
		Telemetry::printf(MSG_INFO, "Altimeter detected: ICP-%s\n", buffer);
	}
	else
	{
		Telemetry::printf(MSG_ERROR, "Altimeter NOT detected!\n");
		return;
	}

	send16BitWord(0x805D); // soft reset
	if(!updateOPT()) Telemetry::printf(MSG_ERROR, "Unable to get Altimeter OPT params!\n");
}

bool Altimeter::updateOPT()
{
	// get OPT calibration parameters

	send16BitWord(0xC595); // move register pointer
	send16BitWordWithCRC(0x0066, 0x9C); // set up OPT Read
	for(int i=0; i<4; i++)
	{
		send16BitWord(0xC7F7); // increment OPT register pointer
		uint8_t cnbuf[2] = {0};
		if(readBytes(cnbuf, 2))
			cn[i] = cnbuf[0] << 8 | cnbuf[1];
		else
			return false;
	}
	return true;
}

float Altimeter::getAlittude()
{
	float altitudeResult = 0;
	for(int i=0; i<ALTITUDE_FILTER_TAP_NUM; i++)
	{
		uint8_t ringIndex=(i+altitudeFilterRingBufferIndex)%ALTITUDE_FILTER_TAP_NUM;
		altitudeResult+=ALTITUDE_FILTER_TAPS[i]*altitudeFilterRingBuffer[ringIndex];
	}
	return altitudeResult;
}

float Altimeter::getVelocity()
{
	float velocityResult = 0;
	for(int i=0; i<VELOCITY_FILTER_TAP_NUM; i++)
	{
		uint8_t ringIndex=(i+velocityFilterRingBufferIndex)%VELOCITY_FILTER_TAP_NUM;
		velocityResult+=VELOCITY_FILTER_TAPS[i]*velocityFilterRingBuffer[ringIndex];
	}
	return velocityResult;
}

void Altimeter::getNewSample()
{
	if(send16BitWord(ALTIMITER_MODE_LOW_POWER)) return; // checking for ACK at end of transmission
	for(int i=0; i<10; i++) {
		//try to read from altimeter
		delayMicroseconds(100);
		uint8_t buf[6] = {0};
		if(readBytes(buf, 2) && readBytes(buf+2, 4))
		{
			uint8_t T_MSB = buf[0];
			uint8_t T_LSB = buf[1];
			uint8_t P_MMSB = buf[2+0];
			uint8_t P_MLSB = buf[2+1];
			uint8_t P_LMSB = buf[2+2];
			// uint8_t P_LLSB = buf[2+3]; we dont use this byte..., cause we dont need it!
			p_dout = P_MMSB << 16 | P_MLSB << 8 | P_LMSB;
			t_dout = T_MSB << 8 | T_LSB;

			float t = (float)(t_dout - 32768);
			float s1 = LUT_lower + (float)(cn[0] * t * t) * quadr_factor;
			float s2 = offst_factor * cn[3] + (float)(cn[1] * t * t) * quadr_factor;
			float s3 = LUT_upper + (float)(cn[2] * t * t) * quadr_factor;
			float C = (s1 * s2 * (p_Pa_calib_0 - p_Pa_calib_1) + s2 * s3 * (p_Pa_calib_1 - p_Pa_calib_2) +
			s3 * s1 * (p_Pa_calib_2 - p_Pa_calib_0)) /
			(s3 * (p_Pa_calib_0 - p_Pa_calib_1) +
			s1 * (p_Pa_calib_1 - p_Pa_calib_2) +
			s2 * (p_Pa_calib_2 - p_Pa_calib_0));
			float A = (p_Pa_calib_0 * s1 - p_Pa_calib_1 * s2 - (p_Pa_calib_1 - p_Pa_calib_0) * C) / (s1 - s2);
			float B = (p_Pa_calib_0 - A) * (s1 + C);

			pressure = A + B/(C+p_dout);
			temperatureC = -45 + 175.0/(float)(1<<16) * t_dout;

			// Note, this function is the inverse of the Barometric Formula
			// See https://en.wikipedia.org/wiki/Barometric_formula for definition
			// also see scripts/altitudecalc.m for a MATLAB script to test this function
			float newAlt;
			if (pressure > 22632.0)
			{
				const float L=-0.0065;
				const float Tb=288.15;
				const float Ps=101325.00;
				newAlt = Tb/(L*expf( logf(pressure/Ps) / ((CONST_G*CONST_M)/(CONST_R*L))))-Tb/L;
			}
			else if(pressure > 5474.90)
			{
				const float H=11000;
				const float Tb=216.65;
				const float Ps=22632.10;
				newAlt = -Tb*CONST_R*logf(pressure/Ps)/(CONST_G*CONST_M)+H;
			}
			else
			{
				const float  H=20000;
				const float L=0.001;
				const float Tb=216.65;
				const float Ps=5474.89;
				newAlt = Tb/(L*expf( logf(pressure/Ps) / ((CONST_G*CONST_M)/(CONST_R*L))))-Tb/L+H;
			}
			float lastAltitude = altitudeFilterRingBuffer[altitudeFilterRingBufferIndex];
			uint32_t newTime = micros();
			uint32_t diff = newTime-lastSampleTime;
			if(diff!=0)
			{
				velocityFilterRingBufferIndex++;
				if(velocityFilterRingBufferIndex>=VELOCITY_FILTER_TAP_NUM) velocityFilterRingBufferIndex = 0;
				velocityFilterRingBuffer[velocityFilterRingBufferIndex] = (newAlt-lastAltitude)/(float)(diff);
			}
			altitudeFilterRingBufferIndex++;
			if(altitudeFilterRingBufferIndex>=ALTITUDE_FILTER_TAP_NUM) altitudeFilterRingBufferIndex = 0;
			altitudeFilterRingBuffer[altitudeFilterRingBufferIndex] = newAlt;
		}
	}

}

float Altimeter::getTempC()
{
	return temperatureC;
}

float Altimeter::getTempK()
{
	return temperatureC - 273.15;
}

float Altimeter::getTempF()
{
	return 1.8*temperatureC + 32;
}

uint8_t Altimeter::send8BitWord(uint8_t word)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send(word & 0xFF);
	return Wire.endTransmission();
}

uint8_t Altimeter::send8BitWordWithCRC(uint8_t word, uint8_t crc)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send(word & 0xFF);
	Wire.send(crc);
	return Wire.endTransmission();
}


uint8_t Altimeter::send8BitWordWithCRC(uint8_t word)
{
	uint8_t wordbytes[1] = { word & 0xFF };
	uint8_t crc = generateCRC8(wordbytes, 1);
	return send8BitWordWithCRC(word, crc);
}

uint8_t Altimeter::send16BitWord(uint16_t word)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send((word >> 8) & 0xFF);
	Wire.send(word & 0xFF);
	return Wire.endTransmission();
}

uint8_t Altimeter::send16BitWordWithCRC(uint16_t word, uint8_t crc)
{
	Wire.beginTransmission(ALTIMETER_ADDRESS);
	Wire.send((word >> 8) & 0xFF);
	Wire.send(word & 0xFF);
	Wire.send(crc);
	return Wire.endTransmission();
}


uint8_t Altimeter::send16BitWordWithCRC(uint16_t word)
{
	uint8_t wordbytes[2] = {(word >> 8) & 0xFF, word & 0xFF};
	uint8_t crc = generateCRC8(wordbytes, 2);
	return(send16BitWordWithCRC(word, crc));
}

bool Altimeter::readBytes(uint8_t* buf, uint8_t len)
{
	Wire.requestFrom((unsigned char)ALTIMETER_ADDRESS, (unsigned char)len);
	if(Wire.available() >= len+1)
	{
		for(int i=0; i<len; i++)
		{
			buf[i] = Wire.read();
		}
		uint8_t crc = Wire.read();
		if(generateCRC8(buf, len) != crc)
		{
			Telemetry::printf(MSG_ERROR, "Altimeter CRC ERROR!\n");
			return false;
		}
		return true;
	}
	return false;
}

uint8_t Altimeter::generateCRC8(const uint8_t *bytes, uint8_t length)
{
	return CRC8.generic(0x31, 0xFF, CRC_FLAG_NOREFLECT, bytes, length);
}
