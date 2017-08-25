
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
#define TURN_SPEED 20 // 旋回時の出力
#define STATION_ANGLE 90 // 右ブロック方向
#define SWITCH_OFF_ANGLE 45 // スイッチを切れる角度
#define FORWARD_ANGLE 180 // 正面方向

void ETTrainStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager *IOManager = InOutManager::GetInstance();
	SelfPositionManager *SpManager = SelfPositionManager::GetInstance();
	PIDData pid = PIDDataManager::GetInstance()->GetPIDData(ETSumoPIDState);
	int currentAngle = SpManager->RobotAngle;
	Point currentPoint = SpManager->RobotPoint;

ACTION :
	switch (CurrentState) {
	// ゴール後、駅の前まで後退中
	case BackToStation:
		// 駅の前に到達したので、駅に尻尾を向けるまで旋回中に遷移
		if (currentPoint.X > 1600) {
			CurrentState = TurnToStation;
			goto ACTION;
		}
		// 前に到達するまで後退
		IOManager->Back(BACK_POWER);
		break;

	// 駅に尻尾を向けるまで旋回中
	case TurnToStation:
		if (abs(currentAngle - STATION_ANGLE) < 10) {
			CurrentState = TurnOffSwitch;
			// 遷移時に、尻尾を下げる
			IOManager->DownTailMotor();
			goto ACTION;
		}
		// 駅角度まで旋回
		IOManager->Turn(currentAngle, STATION_ANGLE, TURN_SPEED);
		break;

	// スイッチを操作して、停止側にする
	case TurnOffSwitch:
		if (abs(currentAngle - SWITCH_OFF_ANGLE) < 10){
			CurrentState = TurnFront;
			// 遷移時に、尻尾を上げる
			IOManager->UpTailMotor();
			goto ACTION;
		}
		// スイッチオフ角度まで旋回
		IOManager->Turn(currentAngle, SWITCH_OFF_ANGLE, TURN_SPEED);
		break;

	// 進行方向を向くまで旋回する
	case TurnFront:
		if (abs(currentAngle - FORWARD_ANGLE) < 10){
			CurrentState = LineTraceToArena;
			goto ACTION;
		}
		// 進行方向まで旋回
		IOManager->Turn(currentAngle, FORWARD_ANGLE, TURN_SPEED);
		break;

	// たどり着くまで、ライントレースを実施する
	case LineTraceToArena:
		IOManager->LineTraceAction(pid, 80, true);
		break;
	// 土俵入りする
	case UpToArena:
		// 乗ったら、戦略クラスをET相撲戦略に変更
		if(false) {
			Manager->SetStrategy(new ETSumoStrategy(Manager));
		}
	default:
		break;
	}
}