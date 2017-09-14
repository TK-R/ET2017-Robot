#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "PrizeStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialData.h"
#include "PIDDataManager.h"
#include "StrategyManager.h"

#define OSHIDASHI_SPEED 20 // 押し出し動作時のスピード

#define FIRST_TURN_SPEED 40 // 高速旋回時の出力
#define TURN_SPEED 25 // 旋回時の出力

#define BACK_ANGLE 180   // 後退方向
#define FORWARD_ANGLE 1 // 正面方向
#define PRIZE_ANGLE 270 // 右ブロック方向

#define ONLINE 25	  // 黒線上での輝度値
#define NotONLINE 60  // 黒線以外での輝度値
#define EDGE_LINE 120 // 黒線との境界線

#define NOT_BLOCK_DISTANCE 6 // ブロックを認識していないときの距離
#define DETECT_PRIZE_DISTANCE 5	 // ブロックを認識した時のきょり　

void PrizeStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager *IOManager = InOutManager::GetInstance();
	SelfPositionManager *SpManager = SelfPositionManager::GetInstance();

	// 懸賞運び中は、パーティクルフィルタOFF
	SpManager->ParticleFilterON = false;
ACTION :

	PIDData pid = PIDDataManager::GetInstance()->GetPIDData(ETSumoPIDState);
	int currentAngle = SpManager->RobotAngle;
	Point currentPoint = SpManager->RobotPoint;


	switch (CurrentState) {
	// 懸賞前の黒線の向きに旋回
	case TurnOnLine:
		if(currentAngle > BACK_ANGLE){
			// 黒線方向を向いたら、ライントレースに遷移
			CurrentState = ForwardLineTrace;
			goto ACTION;
		}
		IOManager->TurnCCW(TURN_SPEED);
		break;
	
	// 懸賞前に移動
	case ForwardLineTrace:
		// 指定座標に到達したら、懸賞の向きに旋回
		if(currentPoint.X < 2310) {
			CurrentState = TurnToPrize;	
			// ライントレース中なので、角度を修正
			SpManager->ResetAngle(BACK_ANGLE);
		}
		IOManager->LineTraceAction(pid, 120, true);

		break;
	
	// 懸賞の方向に旋回
	case TurnToPrize:
		if(currentAngle > (PRIZE_ANGLE - 4)) {
			// アームを下げるまで、いったん停止する
			IOManager->Stop();
			IOManager->WriteOutputMotor();

			// アームを下げて、直進状態に遷移
			IOManager->ARMMotorAtBottom();
			CurrentState = ForwardPrize;
			goto ACTION;
		}

		IOManager->TurnCCW(TURN_SPEED);
		break;

	// 懸賞まで直進
	case ForwardPrize:
		// 懸賞を認識したら、アームを上げて後退状態に遷移する
		if(IOManager->InputData.SonarDistance <= DETECT_PRIZE_DISTANCE){
			// 追加で0.3秒間だけ進む
			dly_tsk(300);

			// アームを上げるまで、いったん停止する
			IOManager->Stop();
			IOManager->WriteOutputMotor();	

			// アームを上げて、後退状態に遷移
			IOManager->UpARMMotor();
			IOManager->TopARMMotor();
			CurrentState = BackPrize;
			break;
		}
		
		// 懸賞まで直進
		IOManager->Forward(pid.BasePower);
		break;

	case BackPrize:
		// 一定距離後退したら、懸賞置き場に向かう直線の方向に旋回
		if(currentPoint.Y < 2950) {
			CurrentState = TurnPrizePlaceLine;
			goto ACTION;
		}

		IOManager->Back(pid.BasePower);
		break;
	
	case TurnPrizePlaceLine:
		// 直線の方向に旋回したら、懸賞置き場まで移動する
		if(currentAngle < 45 && currentAngle > 2) {
			CurrentState = ForwardPrizePlace;
			goto ACTION;
		}

		IOManager->TurnCCW(TURN_SPEED);
		break;

	case ForwardPrizePlace:
		// 懸賞置き場の前まで移動したら、懸賞置き場の方向を向く
		if(currentPoint.X > 2870) {
			CurrentState = TurnPrizePlace;
			goto ACTION;
		}

		IOManager->Forward(pid.BasePower);
		break;
	
	case TurnPrizePlace:
		// 懸賞置き場の方向を向いたら、最後の直進を実施する
		if(currentAngle > 90 && currentAngle < (PRIZE_ANGLE + 5)) {
			CurrentState = LastForwardPrizePlace;
			SpManager->Distance = 0;
			goto ACTION;
		}
		 
		IOManager->TurnCW(TURN_SPEED);
		break;

	case LastForwardPrizePlace:
		if(SpManager->Distance > 85) {
			// アームを下げて後退する
			SpManager->Distance = 1000;
			IOManager->Stop();
			IOManager->WriteOutputMotor();
			IOManager->DownARMMotor();	

			CurrentState = BackLastLine;
			goto ACTION;
		}

		IOManager->Forward(10);
		break;

	case BackLastLine:
		// 7cmほど後退して、黒線を認識するまで後退
		if(SpManager->Distance < 930 && IOManager->InputData.ReflectLight < 120) {
			CurrentState = TurnGurage; 
			goto ACTION;
		}

		IOManager->Back(20);
		break;
	case TurnGurage:
		if(currentAngle < 180) {
			CurrentState = ForwardGurage;
			SpManager->Distance = 0;
			goto ACTION;
		}

		IOManager->TurnCCW(TURN_SPEED);
		break;
	case ForwardGurage:
		if(SpManager->Distance > 840) {
			IOManager->Stop();
							
			Manager->SetStrategy(NULL);
			break;
		}
		
		IOManager->LineTraceAction(PIDDataManager::GetInstance()->GetPIDData(ETSumoPIDState), 120, true);
		break;
	default:
		break;
	}
}