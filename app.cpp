
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
    pData.Angle = 90;
    pData.PositionX = 100;
    pData.PositionY = 200;

    SelfPositionManager* SpManager = new SelfPositionManager(pData);

    static char buf[256];
    int i = 0;

    while(1)
    {
        // 入力情報更新
        IOManager->ReadInputSensor();
        pData.PositionX+=1;
        pData.PositionY+=1;
        pData.Angle+=1;
        SpManager->ResetPosition(pData);
        SpManager->UpdatePosition(0, 0);
        
        StManager->Run();

        sprintf(buf, "%d, Power: %4d, %4d ",i, IOManager->OutputData.LeftMotorPower, IOManager->OutputData.RightMotorPower);
        ev3_lcd_draw_string(buf, 0, 0);

        sprintf(buf, "X:%5d, Y:%5d ",pData.PositionX, pData.PositionY);
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
