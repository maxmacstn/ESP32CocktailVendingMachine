#ifndef VendingController_H
#define VendingController_H

#include <Arduino.h>

class VendingController
{

  public:
    unsigned long int dispStopTime[3];
    unsigned long int dispStartTime = 0;
    const float flowRate = 25; // 1560 ml/minute -> 26 ml/sec
    
    // setting PWM properties
    const int freq = 30000;
    const int resolution = 8;
    int dutyCycle = 200;

    uint8_t disp_pin[];

    VendingController(uint8_t disp1_pin, uint8_t disp2_pin, uint8_t disp3_pin);
    void dispense(int disp1_ml, int disp2_ml, int disp3_ml);
    void run();
    int getProgress();

  private:
    int getDispenseTime(int ml);
};

#endif
