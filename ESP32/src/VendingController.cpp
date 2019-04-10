#include "VendingController.h"
#include "math.h"

//Init variable and setup PWM Pin
VendingController::VendingController(uint8_t disp1_pin, uint8_t disp2_pin, uint8_t disp3_pin)
{
    disp_pin[0] = disp1_pin;
    disp_pin[1] = disp2_pin;
    disp_pin[2] = disp3_pin;
    for(int i = 0; i < 3; i++){
        pinMode(disp_pin[i],OUTPUT);
        ledcSetup(i+1, freq, resolution);       //Setup PWM Pin
          ledcAttachPin(disp_pin[i], i+1);      //Attach PWM Pin to PWM Channel

    }
}

//Set the dispenser to dispense by volume
void VendingController::dispense(int disp1_ml, int disp2_ml, int disp3_ml)
{
    //The machine calculate end time for all dispenser
    Serial.println("VendingCtrl : " + String(disp1_ml, DEC) + " " + String(disp2_ml, DEC) + " "+ String(disp3_ml, DEC));
    dispStopTime[0] = millis() + getDispenseTime(disp1_ml);
    dispStopTime[1] = millis() + getDispenseTime(disp2_ml);
    dispStopTime[2] = millis() + getDispenseTime(disp3_ml);
    dispStartTime = millis();

    for(int i = 0; i < 3; i++){
    Serial.println("VendingCtrl : " + String(dispStopTime[0], DEC) + " " + String(dispStopTime[1], DEC) + " "+ String(dispStopTime[2], DEC));
    }
}

//Get the time that the dispenser need to turn on by volume
//Return: millis of dispense time
int VendingController::getDispenseTime(int ml)
{
    return (ml / flowRate) * 1000;
}

//Check periodically if the pump should run or stop
void VendingController::run()
{
    static bool isOn[] = {false,false,false};

    for (int i = 0; i < 3; i++)
    {
        //Not the stop time, then turn on pump
        if (dispStopTime[i] > millis()){
            if (!isOn[i]){
                //digitalWrite(disp_pin[i], HIGH);
                ledcWrite(i+1 ,200);
                isOn[i] = 1;
            }
        }       
        //Stop time passed, turn off pump.
        else{
            //digitalWrite(disp_pin[i], LOW);
            if (isOn[i]){
            ledcWrite(i +1 ,0);
            isOn[i] = 0;
            }


        }
    }
}

//Calculate percent progress of dispensing progress
int VendingController::getProgress()
{
    int min_percent = INT_MAX;

    for (int i = 0; i < 3; i++){

        if (dispStopTime[i] > millis())
        {
            min_percent = min(100 - (int)((((float)dispStopTime[i] - millis()) / (dispStopTime[i] - dispStartTime)) * 100), min_percent);
        }
        else
        {
            min_percent = min(min_percent, 100);
        }
    }

    return min_percent;
}


    