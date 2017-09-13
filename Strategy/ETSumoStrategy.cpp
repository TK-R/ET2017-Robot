#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "ETSumoStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialData.h"
#include "PIDDataManager.h"
#include "Strategy.h"
#include "StrategyManager.h"
#include "PrizeStrategy.h"

#define OSHIDASHI_SPEED 20 // 押し出し動作時のスピード

#define FIRST_TURN_SPEED 32 // 高速旋回時の出力
#define TURN_SPEED 20 // 旋回時の出力

#define LEFT_ANGLE 90   // 左ブロック方向
#define RIGHT_ANGLE 270 // 右ブロック方向
#define FORWARD_ANGLE 1 // 正面方向

#define ONLINE 25	  // 黒線上での輝度値
#define NotONLINE 60  // 黒線以外での輝度値
//#define EDGE_LINE 120 // 黒線との境界線
#define EDGE_LINE 109 // 黒線との境界線

#define NOT_BLOCK_DISTANCE 8 // ブロックを認識していないときの距離
#define BLOCK_DISTANCE 3	 // ブロックを認識した時の距離　

void ETSumoStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager *IOManager = InOutManager::GetInstance();
	SelfPositionManager *SpManager = SelfPositionManager::GetInstance();
	HSLColorKind DetectColor = IOManager->HSLKind;
	
	SpManager->ParticleFilterON = false;
ACTION :

	PIDData pid = PIDDataManager::GetInstance()->GetPIDData(ETSumoPIDState);
	PIDData pidH = PIDDataManager::GetInstance()->GetPIDData(ETSumoHighPIDState);
	
	int currentAngle = SpManager->RobotAngle;
	Point currentPoint = SpManager->RobotPoint;

	switch (CurrentState) {
	// 初回土俵入り時
	case FirstOnArena:
		// 黒線認識したら、初回黒線横断中に遷移
		if (IOManager->InputData.ReflectLight < ONLINE) {
			// ライントレース中なので、角度は常にライン方向
			SpManager->ResetAngle(FORWARD_ANGLE);
			SpManager->ResetY(2920);

			CurrentState = FirstOverLine;
			goto ACTION;
		}
		// 黒線まではラインの右エッジに沿ってライントレース
		if(CurrentArena == 0) {
			IOManager->LineTraceAction(pidH, EDGE_LINE, false);
		} else {
			IOManager->LineTraceAction(pid, EDGE_LINE, false);		
		}
		break;

	// 初回黒線横断時
	case FirstOverLine:
		// 黒線を抜けたら、初回左ブロック方向旋回状態に遷移
		if (IOManager->InputData.ReflectLight > NotONLINE) {
			CurrentState = FirstTurnLeftPlace;
			goto ACTION;
		}
		// 黒線認識までは普通に直進
		IOManager->Forward(pid.BasePower);
		break;
	
	// 初回左ブロック旋回中
	case FirstTurnLeftPlace:
		// 左ブロック方向を向いたら、左ブロックまで直進中に遷移
		if(abs(currentAngle - LEFT_ANGLE) < 5){
			CurrentState = ForwardLeftPlace;
			goto ACTION;
		}
		
		IOManager->OutputData.LeftMotorPower = -1 * 0.25 * TURN_SPEED;
		IOManager->OutputData.RightMotorPower = TURN_SPEED;
		break;

		// 左ブロックまで直進
	case ForwardLeftPlace:
		// 超音波センサでブロックを認識した場合には、色認識に移行
		if (IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
			CurrentState = DetectLeftBlock;
			// ブロック位置に到達したので、座標を修正
			SpManager->ResetY(2790);
			IOManager->Stop();
			break;
		}

		// ラインの右側をトレースしながらブロックに接近
		IOManager->LineTraceAction(pid, EDGE_LINE, false);

		// ライントレース中なので、角度は常にライン沿い
		SpManager->ResetAngle(LEFT_ANGLE);
		break;

	// 左ブロック色認識
	case DetectLeftBlock:
		// アームを上昇させて、色認識種別をブロックに変更
		IOManager->UpARMMotor();
		IOManager->HSLTargetType = BlockColor;

		// 色が一致している場合には、カウントを進める
		if (DetectColor == PrevColor && DetectColor != HSLBlack) {
			ColorDetectCount++;
			if (ColorDetectCount > 5) {
				ColorDetectCount = 0;
				// 色認識が完了したため、アームを下降させる
				IOManager->DownARMMotor();
				// フィールドの色と土俵の色が一致した場合、寄り切り
				if (ArenaArray[CurrentArena].LeftPlaceColor == DetectColor) {
					CurrentState = YORIKIRILeft;
				} else {
					// フィールドの色と土俵の色が異なる場合、押し出し
					CurrentState = OSHIDASHILeft;
				}
				break;
			}
		} else {
			PrevColor = DetectColor;
			ColorDetectCount = 0;
		}
		break;

	// 左ブロックを寄り切り
	case YORIKIRILeft:
		// ある程度旋回してから、黒線上に乗ったら右ブロックへの直進状態に移行
		if (IOManager->InputData.ReflectLight < EDGE_LINE &&
			abs(currentAngle - RIGHT_ANGLE) < 45) {
			CurrentState = ForwardRightPlace;
			goto ACTION;
		} else if (abs(currentAngle - RIGHT_ANGLE) < 25) {
			// 右ブロック方向を向くまで旋回
			IOManager->TurnCW(TURN_SPEED);
		} else {
			// 右ブロック方向を向くまで旋回（離れているため高速
			IOManager->TurnCW(FIRST_TURN_SPEED);
		}
		break;

	// 左ブロックを押し出し
	case OSHIDASHILeft:
		// 一定距離進んだら後退
		if(currentPoint.Y < 2750) {
			CurrentState = OSHIDASHILeftBack;
			IOManager->UpARMMotor();
			goto ACTION;			
		}
		IOManager->Forward(OSHIDASHI_SPEED);
		break;
	case OSHIDASHILeftBack:
		// 一定距離後退したら、以降は寄り切りと一緒
		if(currentPoint.Y > 2775){
			IOManager->DownARMMotor();
			CurrentState = YORIKIRILeft;
			goto ACTION;			
		}
		IOManager->Back(pid.BasePower);
		break;
	/// 右ブロックまで前進
	case ForwardRightPlace:
		// 超音波センサでブロックを認識した場合には、色認識に移行
		if (IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
			CurrentState = DetectRightBlock;
			// 右ブロックを検知したため、座標を修正
			SpManager->ResetY(3050);
			IOManager->Stop();
			break;
		}

		// ラインに沿って右ブロック置き場に接近
		IOManager->LineTraceAction(pid, EDGE_LINE, true);
		// ライントレース中なので常に角度はライン沿い
		SpManager->ResetAngle(RIGHT_ANGLE);

		break;
	// 右ブロック色認識
	case DetectRightBlock:
		// アームを上昇させて、色認識種別をブロックに変更
		IOManager->UpARMMotor();
		IOManager->HSLTargetType = BlockColor;

		// 色が一致している場合には、カウントを進める
		if (DetectColor == PrevColor && DetectColor != HSLBlack) {
			ColorDetectCount++;
			if (ColorDetectCount > 10) {
				ColorDetectCount = 0;
				// 色認識が完了したため、アームを下降させる
				IOManager->DownARMMotor();
				// フィールドの色と土俵の色が一致した場合、寄り切り
				if (ArenaArray[CurrentArena].RightPlaceColor == DetectColor) {
					CurrentState = YORIKIRIRight;
				} else {
					// フィールドの色と土俵の色が異なる場合、押し出し
					CurrentState = OSHIDASHIRight;
				}
				break;
			}
		} else {
			// 色が異なった場合には再度カウント
			PrevColor = DetectColor;
			ColorDetectCount = 0;
		}
		break;

	// 右ブロックを寄り切り
	case YORIKIRIRight:
		// 斜め方向を向いたらフィールド走行状態に遷移
		if (abs(currentAngle - (LEFT_ANGLE)) < 10) {
			CurrentState = ForwardCenterFromRight;
			goto ACTION;			
		} else if(abs(currentAngle - (LEFT_ANGLE)) < 25) {
			// 中央方向を向くまで旋回
			IOManager->TurnCCW(TURN_SPEED);
		} else {
			// 中央方向を向くまで旋回 (離れているので高速)
			IOManager->TurnCCW(FIRST_TURN_SPEED);			
		}
		break;

	// 右ブロックを押し出し
	case OSHIDASHIRight:
		// 一定距離進んだら後退
		if(currentPoint.Y > 3090) {
			IOManager->UpARMMotor();
			CurrentState = OSHIDASHIRightBack;
			goto ACTION;			
		}
		IOManager->Forward(OSHIDASHI_SPEED);
		break;
	case OSHIDASHIRightBack:
		// 一定距離後退したら、以降は寄り切りと一緒
		if(currentPoint.Y < 3070){
			IOManager->DownARMMotor();

			CurrentState = YORIKIRIRight;
			goto ACTION;			
		}
		IOManager->Back(pid.BasePower);
		break;
	
	// 中央方向へ前進
	case ForwardCenterFromRight:
		// 黒線認識したら、前方旋回中状態に遷移
		if (IOManager->InputData.ReflectLight < ONLINE) {
			// 最後の土俵の場合には、懸賞運搬戦略に遷移
			if(CurrentArena == 3) {
				// 座標をクリア
				SpManager->ResetX(2470);
				SpManager->ResetY(2900);
				SpManager->ResetAngle(LEFT_ANGLE);
				
				IOManager->Stop();
				IOManager->WriteOutputMotor();
			    dly_tsk(100);
	
				Manager->SetStrategy(new PrizeStrategy(Manager));
				break;
			}
		
			CurrentState = TurnForward;
			SpManager->ResetAngle(LEFT_ANGLE);
			goto ACTION;
		}
		IOManager->LineTraceAction(pid, EDGE_LINE, false);		
		break;

	case TurnForward:
		// 黒線上に乗ったら一枚の土俵攻略が終了
//		if (currentAngle > 305 && currentAngle < 355) {
		if (currentAngle < 6 || currentAngle > 350) {	
			CurrentArena++;
				
			// 2枚目の攻略が完了した場合には、線路をまたぐ直進処理に遷移
			if(CurrentArena == 2) {
				SpManager->Distance = 0;
				CurrentState = ForwardRail;
				// 座標修正
				SpManager->ResetX(1920);
				goto ACTION;					
			} else {
				// 2枚目でなければ、通常の処理に遷移
				CurrentState = FirstOnArena;
				goto ACTION;					
			}
			break;
		}

		// 正面を向くまで旋回	
		IOManager->OutputData.LeftMotorPower = TURN_SPEED;
		IOManager->OutputData.RightMotorPower =  -1 * 0.5 * TURN_SPEED;

		break;

	case ForwardRail:
		if(currentPoint.X > 2120) {
			// 線路を跨いだので、通常のライントレースに遷移
			CurrentState = FirstOnArena;
			goto ACTION;
		}

		// 最初はライントレース
		if(SpManager->Distance < 90) {
			IOManager->LineTraceAction(pid, EDGE_LINE, false);
		} else if(SpManager->Distance < 130){
			IOManager->Forward(25);	
		} else {
			IOManager->LineTraceAction(pid, EDGE_LINE, false);		
		}

		break;
	default:
		break;
	}
}