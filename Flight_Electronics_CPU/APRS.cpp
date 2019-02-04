/*
 * APRS.cpp
 *
 *  Created on: Feb 4, 2019
 *      Author: erics
 */

#include "APRS.h"

void APRS::init()
{
	  aprs_setup(50, // number of preamble flags to send
		     PTT_PIN, // Use PTT pin
		     100, // ms to wait after PTT to transmit
		     0, 0 // No VOX ton
		     );
}
