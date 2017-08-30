
#include "ev3api.h"
#include "Clock.h"
#include "app.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "StrategyManager.h"
#include "BlockMoveStrategy.h"
#include "ETSumoStrategy.h"
#include "SelfPositionManager.h"
#include "BlockMoveManager.h"
#include "ETTrainStrategy.h"
#include "SoundPlayTask.h"
#include "HSLColor.h"
#include "FieldMap.h"

using ev3api::Clock;

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

// 制御周期
#define BASE_TIME 14

imageData_t* image;

void Draw()
{
    static char buf[256];
    
    InOutManager* IOManager = InOutManager::GetInstance();
    BlockMoveManager * BmManager = BlockMoveManager::GetInstance();
    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
  
    sprintf(buf, "C:%d src:%2d, dst:%2d, Ang:%3d ", 
        BmManager->CurrentCommand.BlockColor,
        BmManager->CurrentCommand.SourceBlockPosition,
        BmManager->CurrentCommand.DestinationBlockPosition,
        BmManager->GetSrcWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y));
    ev3_lcd_draw_string(buf, 0, 0);

    auto src = BmManager->GetSrcWaypoint();
    sprintf(buf, "W* No:%d srcX:%2d, srcY:%2d  ", 
        BmManager->CurrentCommand.ApproachWayPoint[BmManager->CurrentSrcWaypointNo],
        (int)src->X,
        (int)src->Y);
    ev3_lcd_draw_string(buf, 0, 12);

    RGBColor rgbMap = FieldMap::GetInstance()->GetRGBColor(
                            SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
    HSLColor hslMap = HSLColor::FromRGB(rgbMap.R, rgbMap.G, rgbMap.B);

    sprintf(buf, "MAP-H:%3.2f,S:%3.2f,L%3.2f",
        hslMap.Hue,
        hslMap.Saturation,
        hslMap.Luminosity);
    ev3_lcd_draw_string(buf, 0, 24);

    sprintf(buf, "LFood: %1.4f", ColorDecision::GetLikelihoodLuminosity(hslMap.Luminosity, IOManager->HSLValue.Luminosity));
    ev3_lcd_draw_string(buf, 0, 36);
    
}

void Refresh()
{
    InOutManager* IOManager = InOutManager::GetInstance();
    IOManager->ReadInputSensor();

    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    SpManager->UpdatePosition(IOManager->InputData.LeftMotorAngle, IOManager->InputData.RightMotorAngle);   
 //   Draw();
}

void main_task(intptr_t unused) 
{
    PlaySound(SensorInitialStart);
    InOutManager* IOManager = InOutManager::GetInstance();
    FieldMap * Map = FieldMap::GetInstance();
    Map->ReadImage("/ev3rt/image/Field.bmp");
    StrategyManager *StManager = new StrategyManager();
    PlaySound(SensorInitialEnd);    
    
RESTART_:

    // 初期位置
    SelfPositionData pData;
/*
    pData.Angle = 0;
    pData.PositionX = 1360;
    pData.PositionY = 2900;
*/
    // 初期位置（ライントレース）
    pData.Angle = 270;
    pData.PositionX = 4790;
    pData.PositionY = 430;

    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    SpManager->ResetPosition(pData);

    // デバッグ用
//    IOManager->UpARMMotor();
//   IOManager->HSLTargetType = BlockColor;

    while(IOManager->InputData.TouchSensor == 0){
        Refresh();
		dly_tsk(10);
    }

    while(IOManager->InputData.TouchSensor == 1){
        Refresh();
		dly_tsk(10);
    }

    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT,  EV3_LCD_WHITE);
    
    // StManager->SetStrategy(new BlockMoveStrategy(StManager));


    // 初期値をラインの中心として格納
   auto lts = new LineTraceStrategy(StManager);
//   lts->CenterValue = IOManager->InputData.ReflectLight;
    
   lts->CenterValue = 120;
// ライントレース戦略にて動作開始
   StManager->SetStrategy(lts);

    // ET相撲列車停止
    // StManager->SetStrategy(new ETTrainStrategy(StManager));

    // 初期位置（新幹線停止）
    // pData.Angle = 180;
    // pData.PositionX = 1200;
    // pData.PositionY = 2200;
    // SpManager->ResetPosition(pData);

    // 初期位置（ET相撲）
    //StManager->SetStrategy(new ETSumoStrategy(StManager));
    // 初期位置（ET相撲）
    //pData.Angle = 0;
    //pData.PositionX = 1530;
    //pData.PositionY = 2940;
    //SpManager->ResetPosition(pData);
 
    Clock* clock = new Clock();
    while(1)
    {
        uint32_t baseTime = clock->now();
    
        // 入力情報更新
        Refresh();

        StManager->Run();
        
        // ボタン押されたらリスタート
        if(IOManager->InputData.TouchSensor == 1){
            IOManager->OutputData.LeftMotorPower = 0;
            IOManager->OutputData.RightMotorPower = 0;
            IOManager->DownARMMotor();
            IOManager->UpTailMotor();
            IOManager->WriteOutputMotor();

            while(IOManager->InputData.TouchSensor == 1) {
                Refresh();                                
                dly_tsk(10);
            }
            goto RESTART_;
        }
        // 出力情報更新
        IOManager->WriteOutputMotor();
        
        // 制御に掛かった時間を格納
        IOManager->SleepTime = clock->now() - baseTime;

        // 基準周期から、制御に掛かった時間を引く
		dly_tsk(BASE_TIME - IOManager->SleepTime);
    }
}
