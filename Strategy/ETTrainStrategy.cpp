
#include "Strategy.h"
#include "ETSumoStrategy.h"
#include "StrategyManager.h"
#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "ETTrainStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialData.h"
#include "PIDDataManager.h"

#define BACK_POWER 20
#define TURN_SPEED 12 		// 旋回時の出力
#define FIRST_TURN_SPEED 20	
#define BACK_ANGLE 1
#define ONLINE 60
#define STATION_ANGLE 90 // 駅に尻尾を向けた方向
#define SWITCH_OFF_ANGLE 30 // スイッチを切れる角度
#define FORWARD_ANGLE 180 // 正面方向

//#define BASE_DISTANCE 452
#define BASE_DISTANCE 435


void ETTrainStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager *IOManager = InOutManager::GetInstance();
	SelfPositionManager *SpManager = SelfPositionManager::GetInstance();
	auto pid = PIDDataManager::GetInstance();
	SpManager->ParticleFilterON = false;

ACTION :

	int currentAngle = SpManager->RobotAngle;
	Point currentPoint = SpManager->RobotPoint;

	switch (CurrentState) {
	// ゴール後、反対向きに旋回する
	case TurnToBack:
		if (abs(currentAngle - BACK_ANGLE) < 10) {
			CurrentState = BackToStation;
			// 距離を初期化
			SpManager->Distance = 0;
			goto ACTION;
		}
		IOManager->TurnCCW(FIRST_TURN_SPEED);
		break;

	// 駅の前まで移動中
	case BackToStation:
		// 駅の前に到達したので、駅に尻尾を向けるまで旋回中に遷移
		if (SpManager->Distance > (BASE_DISTANCE + KillCount * 16)) {
			CurrentState = TurnToStation;
			// ライントレース終了時にはまっすぐ後ろ向き
			SpManager->ResetAngle(BACK_ANGLE);
			goto ACTION;
		}
		// 前に到達するまで後退
		IOManager->LineTraceAction(pid->GetPIDData(ETTrainSlow), 100, false);		
		break;

	// 駅に尻尾を向けるまで旋回中
	case TurnToStation:
		if (abs(currentAngle - STATION_ANGLE) < 5) {
			CurrentState = TurnOffSwitch;
			
			// 尻尾を下げている間、スレッドが停止するため強制的に周期書き込みを発生させる
			IOManager->Stop();
			IOManager->WriteOutputMotor();
			// 遷移時に、尻尾を下げる
			IOManager->DownTailMotor();
			goto ACTION;
		}
		// 駅角度まで旋回
		IOManager->Turn(currentAngle, STATION_ANGLE, FIRST_TURN_SPEED);
		break;

	// スイッチを操作して、停止側にする
	case TurnOffSwitch:
		if (abs(currentAngle - SWITCH_OFF_ANGLE) < 5){
			// 尻尾をあげている間、スレッドが停止するため強制的に周期書き込みを発生させる
			IOManager->Stop();
			IOManager->WriteOutputMotor();

			// 遷移時に、尻尾を上げる
			IOManager->UpTailMotor();
			
			KillCount++;
			// 3回実行したら前を向く
			if(KillCount == 3) CurrentState = TurnFront;
			else CurrentState = TurnNextBack;

			goto ACTION;
		}
		// スイッチオフ角度まで旋回
		IOManager->Turn(currentAngle, SWITCH_OFF_ANGLE, TURN_SPEED);
		break;
	
	// もう一度
	case TurnNextBack:
		if (abs(currentAngle - BACK_ANGLE) < 10) {
			CurrentState = BackToStation;
			goto ACTION;
		}
		IOManager->TurnCW(FIRST_TURN_SPEED);
		break;

// 進行方向を向くまで旋回する
	case TurnFront:
		if (IOManager->InputData.ReflectLight < ONLINE && abs(currentAngle - FORWARD_ANGLE) < 60){
			CurrentState = LineTraceSlowToGrayArea;

			IOManager->Stop();
			IOManager->WriteOutputMotor();
			IOManager->BottomTailMotor();
			
			goto ACTION;
		}
		// 進行方向まで旋回
		IOManager->Turn(currentAngle, FORWARD_ANGLE, FIRST_TURN_SPEED);
		break;
	case LineTraceSlowToGrayArea:
		if(currentPoint.X < 1570) {
			CurrentState = LineTraceToGrayArea;
			goto ACTION;
		}
		IOManager->LineTraceAction(pid->GetPIDData(ETTrainSlow), 120, false);
		break;

	case LineTraceToGrayArea:
		// 灰色エリアに到達したら、直進処理に変更
		if(currentPoint.X < 1200) {
			CurrentState = ForwardGrayArea;
			// ライントレース終了時にはまっすぐ前向き
			SpManager->ResetAngle(FORWARD_ANGLE);
			goto ACTION;
		}
		IOManager->LineTraceAction(pid->GetPIDData(ETTrainHigh), 120, false);
		break;

	case ForwardGrayArea:
		// 灰色エリアを抜けたら、ライントレース状態に遷移
		if(currentPoint.X < 1020) {	
			CurrentState = LineTraceToArena;
			goto ACTION;
		}
		IOManager->Forward(pid->GetPIDData(ETTrainHigh).BasePower);
		break;

	// たどり着くまで、ライントレースを実施する
	case LineTraceToArena:
		if(SpManager->Distance > 2680) {
			Manager->SetStrategy(new ETSumoStrategy(Manager));
			return;
		}
		IOManager->LineTraceAction(pid->GetPIDData(ETTrainHigh), 120, false);
		break;

	default:
		break;
	}
}