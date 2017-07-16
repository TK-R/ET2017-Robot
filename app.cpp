
#include "ev3api.h"
#include "app.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"

//#include "libcpp-test.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif



void main_task(intptr_t unused) 
{
    InOutManager* IOManager = new InOutManager();
    

    static char buf[256];
    int i = 0;

            


    while(1)
    {
        IOManager->ReadInputSensor();

        sprintf(buf, "%d, Power: %4d, %4d ",i, OutputData.LeftMotorPower, OutputData.RightMotorPower);
        ev3_lcd_draw_string(buf, 0, 0);

        sprintf(buf, "P: %f, D: %f", CurrentPID.PGain, CurrentPID.DGain);
        ev3_lcd_draw_string(buf, 0, 12);

        if(IOManager->InputData.TouchSensor == 1){
            OutputData.LeftMotorPower = 0;
            OutputData.RightMotorPower = 0;    
        }

        ev3_motor_set_power(EV3_PORT_A, OutputData.LeftMotorPower);
        ev3_motor_set_power(EV3_PORT_B, OutputData.RightMotorPower);

        i++;
		dly_tsk(20);

    }

    //auto obj = new TestClass();
}
