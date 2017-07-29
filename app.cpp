
#include "ev3api.h"
#include "app.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "SelfPositionManager.h"

//#include "libcpp-test.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif



void main_task(intptr_t unused) 
{
    InOutManager* IOManager = InOutManager::GetInstance();
    StrategyManager *StManager = new StrategyManager();
    StManager->SetStrategy(new LineTraceStrategy());

    // 初期位置
    SelfPositionData pData;
    pData.Angle = 270;
    pData.PositionX = 1000;
    pData.PositionY = 1000;

    SelfPositionManager* SpManager = new SelfPositionManager(pData);

    static char buf[256];
    int i = 0;

    while(1)
    {
        // 入力情報更新
        IOManager->ReadInputSensor();
        SpManager->UpdatePosition(IOManager->InputData.LeftMotorAngle, IOManager->InputData.RightMotorAngle);
        
        StManager->Run();

        sprintf(buf, "%d, Power: %4d, %4d ",i, IOManager->OutputData.LeftMotorPower, IOManager->OutputData.RightMotorPower);
        ev3_lcd_draw_string(buf, 0, 0);

        sprintf(buf, "X:%4.1f, Y:%5.1f. A:%3.1f ",SpManager->PositionX, SpManager->PositionY, SpManager->Angle);
        ev3_lcd_draw_string(buf, 0, 12);

        sprintf(buf, "P: %f, D: %f", CurrentPID.PGain, CurrentPID.DGain);
        ev3_lcd_draw_string(buf, 0, 24);

        if(IOManager->InputData.TouchSensor == 1){
            IOManager->OutputData.LeftMotorPower = 0;
            IOManager->OutputData.RightMotorPower = 0;    
        }

        i++;

        // 出力情報更新
        IOManager->WriteOutputMotor();
		dly_tsk(5);

    }

    //auto obj = new TestClass();
}
