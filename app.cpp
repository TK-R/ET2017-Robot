
#include "ev3api.h"
#include "app.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "StrategyManager.h"
#include "BlockMoveStrategy.h"
#include "SelfPositionManager.h"
#include "BlockMoveManager.h"
#include "SoundPlayTask.h"
#include "HSLColor.h"
#include "FieldMap.h"

// C++からC言語ライブラリをインクルードするため
extern "C"{
	#include "image.h"
}


//#include "libcpp-test.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

imageData_t* image;

void Draw()
{
    static char buf[256];
    
    InOutManager* IOManager = InOutManager::GetInstance();

    sprintf(buf, "Power: %4d, %4d ",IOManager->OutputData.LeftMotorPower, IOManager->OutputData.RightMotorPower);
    ev3_lcd_draw_string(buf, 0, 0);

    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    sprintf(buf, "X:%4.1f, Y:%5.1f. A:%3.1f ",SpManager->RobotPoint.X, SpManager->RobotPoint.Y, SpManager->RobotAngle);
    ev3_lcd_draw_string(buf, 0, 12);

    sprintf(buf, "P: %f, D: %f", CurrentPID.PGain, CurrentPID.DGain);
    ev3_lcd_draw_string(buf, 0, 24);

    BlockMoveManager * BmManager = BlockMoveManager::GetInstance();
    sprintf(buf, "C:%d src:%2d, dst:%2d, Ang:%3d", 
        BmManager->CurrentCommand.BlockColor,
        BmManager->CurrentCommand.SourceBlockPosition,
        BmManager->CurrentCommand.DestinationBlockPosition,
        BmManager->GetSrcWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y));
    ev3_lcd_draw_string(buf, 0, 36);

    auto src = BmManager->GetSrcWaypoint();
    sprintf(buf, "W* No:%d srcX:%2d, srcY:%2d", 
        BmManager->CurrentCommand.ApproachWayPoint[BmManager->CurrentSrcWaypointNo],
        (int)src->X,
        (int)src->Y);
    ev3_lcd_draw_string(buf, 0, 48);

    RGBColor* color = FieldMap::GetInstance()->GetRGBColor(
                            SpManager->RobotPoint.X, SpManager->RobotPoint.Y);    
    sprintf(buf, "MAP-R:%d,G:%d,B:%d",
        color->R,
        color->G,
        color->B);
    delete color;
    ev3_lcd_draw_string(buf, 0, 60);
}

void Refresh()
{
    InOutManager* IOManager = InOutManager::GetInstance();
    IOManager->ReadInputSensor();

    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    SpManager->UpdatePosition(IOManager->InputData.LeftMotorAngle, IOManager->InputData.RightMotorAngle);   
    Draw();
}

void main_task(intptr_t unused) 
{
    PlaySound(SensorInitialStart);
    InOutManager* IOManager = InOutManager::GetInstance();
    FieldMap * Map = FieldMap::GetInstance();
    Map->ReadImage("/ev3rt/image/Field.bmp");

    StrategyManager *StManager = new StrategyManager();
    StManager->SetStrategy(new LineTraceStrategy(StManager));

    // 初期位置
    SelfPositionData pData;
    pData.Angle = 90;
    pData.PositionX = 600;
    pData.PositionY = 1200;

    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    SpManager->ResetPosition(pData);
    PlaySound(SensorInitialEnd);

    int i = 0;
    while(IOManager->InputData.TouchSensor == 0){
        Refresh();
		dly_tsk(10);
    }
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT,  EV3_LCD_WHITE);
    
    // StManager->SetStrategy(new BlockMoveStrategy(StManager));

    // 初期値をラインの中心として格納
    auto lts = new LineTraceStrategy(StManager);
    lts->CenterValue = IOManager->InputData.ReflectLight;

    // ライントレース戦略にて動作開始
    StManager->SetStrategy(lts);

    while(1)
    {
        // 入力情報更新
        Refresh();

        StManager->Run();
        
        if(i > 200) {
            i = 0;
            ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT,  EV3_LCD_WHITE);
        }

        if(IOManager->InputData.TouchSensor == 1){
            IOManager->OutputData.LeftMotorPower = 0;
            IOManager->OutputData.RightMotorPower = 0;    
        }
        i++;

        // 出力情報更新
        IOManager->WriteOutputMotor();
		dly_tsk(10);
    }
}
