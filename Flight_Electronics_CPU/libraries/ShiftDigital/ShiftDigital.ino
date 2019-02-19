#include "DigitalShift.h"

void setup() {
 DigitalShift:: init(); // put your setup code here, to run once:

}

void loop() {
   DigitalShift::setOutput(6, 1);
   DigitalShift::setOutput(1, 1);
   DigitalShift::setOutput(8, 1);
   DigitalShift::setOutput(10, 1);
   
   delay (2000);
 
}
