#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "ETSumoStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialData.h"
#include "PIDDataManager.h"

#define OSHIDASHI_SPEED 25 // 押し出し動作時のスピード

#define FIRST_TURN_SPEED 25 // 高速旋回時の出力
#define TURN_SPEED 15 // 旋回時の出力

#define LEFT_ANGLE 90   // 左ブロック方向
#define RIGHT_ANGLE 270 // 右ブロック方向
#define FORWARD_ANGLE 0 // 正面方向

#define ONLINE 25	  // 黒線上での輝度値
#define NotONLINE 60  // 黒線以外での輝度値
#define EDGE_LINE 120 // 黒線との境界線
#define FIELD	  180 // 

#define NOT_BLOCK_DISTANCE 8 // ブロックを認識していないときの距離
#define BLOCK_DISTANCE 4	 // ブロックを認識した時のきょり　

void ETSumoStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager *IOManager = InOutManager::GetInstance();
	SelfPositionManager *SpManager = SelfPositionManager::GetInstance();

	HSLColorKind DetectColor = IOManager->HSLKind;
	PIDData pid = PIDDataManager::GetInstance()->GetPIDData(ETSumoPIDState);
	int currentAngle = SpManager->RobotAngle;
	Point currentPoint = SpManager->RobotPoint;

ACTION :
	switch (CurrentState) {
	// 土俵直進中
	case ForwardArena:
		// 黒線認識したら、黒線横断中状態に遷移
		if (IOManager->InputData.ReflectLight < ONLINE) {
			CurrentState = ForwardOverLeftLine;
			goto ACTION;
		}
		// 黒線まではラインの左側のエッジに沿ってライントレース
		IOManager->LineTraceAction(pid, EDGE_LINE, true);
		// ライントレース中なので、角度は常にライン方向
		SpManager->ResetAngle(FORWARD_ANGLE);
		currentPoint.Y = 2900;
		SpManager->ResetPoint(&currentPoint);
		break;

	// 土俵左黒線横断中
	case ForwardOverLeftLine:
		// 黒線を抜けたら、左ブロック方向旋回状態に遷移
		if (IOManager->InputData.ReflectLight > NotONLINE) {
			CurrentState = TurnLeftPlace;
			goto ACTION;
		}
		// 黒線認識までは普通に直進
		IOManager->Forward(pid.BasePower);
		break;

	// 左ブロック方向旋回
	case TurnLeftPlace:
		// 黒線上に乗ったら左ブロックへの直進状態に移行
		if (IOManager->InputData.ReflectLight < ONLINE &&
						abs(currentAngle - LEFT_ANGLE)) {
			CurrentState = ForwardLeftPlace;
			goto ACTION;
		}

		// 左ブロック方向を向くまで旋回
		//IOManager->Turn(currentAngle, LEFT_ANGLE, TURN_SPEED);
		IOManager->OutputData.LeftMotorPower = -1 * TURN_SPEED / 2;
		IOManager->OutputData.RightMotorPower = TURN_SPEED;

		break;

	// 左ブロックまで直進
	case ForwardLeftPlace:
		// 超音波センサでブロックを認識した場合には、色認識に移行
		if (IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
			CurrentState = DetectLeftBlock;
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
			if (ColorDetectCount > 10) {
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
		if(SpManager->RobotPoint.Y < 2670) {
			CurrentState = OSHIDASHILeftBack;
			goto ACTION;			
		}
		IOManager->Forward(OSHIDASHI_SPEED);
		break;
	case OSHIDASHILeftBack:
		// 一定距離後退したら、以降は寄り切りと一緒
		if(SpManager->RobotPoint.Y > 2750){
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
			IOManager->Stop();
			break;
		}

		// ラインに沿って右ブロック置き場に接近
		IOManager->LineTraceAction(pid, EDGE_LINE, false);
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
		if (abs(currentAngle - (LEFT_ANGLE - 50)) < 5) {
			CurrentState = ForwardField;
			goto ACTION;			
		} else if(abs(currentAngle - (LEFT_ANGLE - 50)) < 15) {
			// 中央方向を向くまで旋回
			IOManager->TurnCW(TURN_SPEED);
		} else {
			// 中央方向を向くまで旋回 (離れているので高速)
			IOManager->TurnCW(FIRST_TURN_SPEED);			
		}
		break;

	// 右ブロックを押し出し
	case OSHIDASHIRight:
		// 一定距離進んだら後退
		if(SpManager->RobotPoint.Y > 3140) {
			CurrentState = OSHIDASHIRightBack;
			goto ACTION;			
		}
		IOManager->Forward(OSHIDASHI_SPEED);
		break;
	case OSHIDASHIRightBack:
		// 一定距離後退したら、以降は寄り切りと一緒
		if(SpManager->RobotPoint.Y < 3070){
			CurrentState = YORIKIRIRight;
			goto ACTION;			
		}
		IOManager->Back(pid.BasePower);
		break;
	
	// フィールド上まで移動
	case ForwardField:
		// 完全に黒線上から移動したら中央方向への移動に遷移
		if(IOManager->InputData.ReflectLight > FIELD) {
			CurrentState = ForwardCenterFromRight;
			goto ACTION;
		}

		IOManager->Forward(pid.BasePower);
		break;
	// 中央方向へ前進
	case ForwardCenterFromRight:
		// 黒線認識したら、黒線横断中状態に遷移
		if (IOManager->InputData.ReflectLight < ONLINE) {
			CurrentState = ForwardOverCenterLineFromRight;
			goto ACTION;
		}

		IOManager->Forward(pid.BasePower);
		
		break;

	case ForwardOverCenterLineFromRight:
		// 黒線を抜けたら、前方方向に旋回状態に遷移
		if (IOManager->InputData.ReflectLight > NotONLINE){
			CurrentState = TurnForward;
			goto ACTION;
		}
		// 黒線認識までは普通に直進
	//	IOManager->LineTraceAction(pid, EDGE_LINE, true);
		IOManager->OutputData.LeftMotorPower = TURN_SPEED;
		IOManager->OutputData.RightMotorPower =  TURN_SPEED / 2;

		break;

	case TurnForward:
		// 黒線上に乗ったら一枚の土俵攻略が終了
	//	if (IOManager->InputData.ReflectLight < EDGE_LINE) {
			//  四枚目の土俵でなければ次の土俵の攻略に遷移
			if (CurrentArena <= 4) {
					CurrentArena++;
					CurrentState = ForwardArena;
					goto ACTION;					
				} else {
				// 全ての土俵の攻略が終わっていれば、ET相撲戦略を終了
				IOManager->Stop();
			}
			break;
	//	}

		// 正面を向くまで旋回	
//		IOManager->TurnCW(TURN_SPEED);
	//	IOManager->OutputData.LeftMotorPower = TURN_SPEED;
	//	IOManager->OutputData.RightMotorPower = 0;
		break;
	default:
		break;
	}
}