
#include "ev3api.h"
#include "Clock.h"
#include "app.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"
#include "Strategy.h"
#include "StrategyManager.h"
#include "LineTraceStrategy.h"
#include "BlockMoveStrategy.h"
#include "ETSumoStrategy.h"
#include "SelfPositionManager.h"
#include "BlockMoveManager.h"
#include "ETTrainStrategy.h"
#include "SoundPlayTask.h"
#include "PrizeStrategy.h"
#include "HSLColor.h"
#include "FieldMap.h"
#include "MotorTestStrategy.h"

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

imageData_t* image;

void Draw()
{
    static char buf[256];
    
    BlockMoveManager * BmManager = BlockMoveManager::GetInstance();
    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
  
    sprintf(buf, "C:%d src:%2d, dst:%2d, Ang:%3d ", 
        BmManager->CurrentCommand.BlockColor,
        BmManager->CurrentCommand.SourceBlockPosition,
        BmManager->CurrentCommand.DestinationBlockPosition,
        BmManager->GetSrcWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y));
    ev3_lcd_draw_string(buf, 0, 0);    
}

void Refresh()
{
    InOutManager* IOManager = InOutManager::GetInstance();
    IOManager->ReadInputSensor();

    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    SpManager->UpdatePosition(IOManager->InputData.LeftMotorAngle, IOManager->InputData.RightMotorAngle);   
//    Draw();
}

void main_task(intptr_t unused) 
{
    PlaySound(SensorInitialStart);
    static char buf[256];
    
    InOutManager* IOManager = InOutManager::GetInstance();
    FieldMap * Map = FieldMap::GetInstance();
    Map->ReadImage("/ev3rt/image/Field.bmp");
    StrategyManager *StManager = new StrategyManager();
    PlaySound(SensorInitialEnd);    
    ev3_lcd_set_font(EV3_FONT_MEDIUM);

RESTART_:
    
    // 初期位置
    SelfPositionData pData;

    // 初期位置（ライントレース）
    pData.Angle = 270;
    pData.PositionY = 430;             
    pData.PositionX = 4790;
    
    SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
    SpManager->ResetPosition(pData);

    auto str = new LineTraceStrategy(StManager);
    // 調整ポイント
    str->CenterValue = 130;

    dly_tsk(100);

    sprintf(buf, "Please Select Course.");
    StManager->SetStrategy(str);    
    
    while(IOManager->InputData.TouchSensor == 0){
        Refresh();
        // Lボタンが押されているときは、Lコースの攻略
        if(ev3_button_is_pressed(LEFT_BUTTON)){
            // ボタンが離されるまで待つ
            while(ev3_button_is_pressed(LEFT_BUTTON)){
                dly_tsk(100);                
            }                  
            
            PlaySound(LCourseStart);

            pData.PositionX = 4790;
            pData.PositionY = 430;             
            pData.Angle = 270;            
            pData.Distance = 0;
            
            str->CurrentState = L_A;

            sprintf(buf, "Course: L-N");
        // Rボタンが押されているときは、Rコースの攻略
        } else if(ev3_button_is_pressed(RIGHT_BUTTON)) {
            // ボタンが離されるまで待つ
            while(ev3_button_is_pressed(RIGHT_BUTTON)){
                dly_tsk(100);                
            }

            PlaySound(RCourseStart);

            pData.PositionX = 5190; 
            pData.PositionY = 430;             
            pData.Angle = 270;            
            pData.Distance = 0;
            
            str->CurrentState = R_A;
            
            sprintf(buf, "Course: R-N");

        // 上ボタンが押されているときには、Lコースの難所攻略
        } else if(ev3_button_is_pressed(UP_BUTTON)){
            // ボタンが離されるまで待つ
            while(ev3_button_is_pressed(UP_BUTTON)){
                dly_tsk(100);                
            }

            pData.Angle = 180;
            pData.PositionX = 1500;
            pData.PositionY = 1780;
            pData.Distance = 10010;
                    
            str->CurrentState = L_A;
            
            sprintf(buf, "Course: L-B");
            
        // 下ボタンが押されているときは、Rコースの難所攻略
        } else if(ev3_button_is_pressed(DOWN_BUTTON)){
            // ボタンが離されるまで待つ
            while(ev3_button_is_pressed(DOWN_BUTTON)){
                dly_tsk(100);                
            }

            pData.Angle = 180;
            pData.PositionX = 1500;
            pData.PositionY = 2200;
            pData.Distance = 10240;
        
            str->CurrentState = R_A;
  
            sprintf(buf, "Course: R-B");            
        }
        
        ev3_lcd_draw_string(buf, 0, 0);
        dly_tsk(100);
    }

    SpManager->ResetPosition(pData);        

    while(IOManager->InputData.TouchSensor == 1){
        Refresh();
		dly_tsk(10);
    }

    // ライントレースを初期化
    IOManager->LineTraceClear(120);

    Clock* clock = new Clock();

    while(1)
    {
        uint32_t baseTime = clock->now();
    
        // 入力情報更新
        Refresh();
        StManager->Run();
        
        // ボタン押されたらリスタート
        if(IOManager->InputData.TouchSensor == 1){
            IOManager->Stop();
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
