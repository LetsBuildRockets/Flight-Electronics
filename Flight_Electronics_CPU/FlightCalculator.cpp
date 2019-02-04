/*
 * FlightCalculator.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: erics
 */

#include "FlightCalculator.h"

float FlightCalculator::calculateExpectedAltitude(float mass, float velocity, float altitude)
{
	// calculate squared and cubed terms
	float m2 = mass*mass;
	float m3 = m2*mass;
	float v2= velocity*velocity;
	float v3 = v2*velocity;
	float x2 = altitude*altitude;
	float x3 = x2*altitude;

	float expectedAltitude = 0;

	// TODO: actual run the polynomial!

	return expectedAltitude;
}
