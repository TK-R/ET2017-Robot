#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "ETSumoStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialData.h"
#include "PIDDataManager.h"


#define FSPEED  		15	// 直進時の出力
#define TURN_SPEED		15	// 旋回時の出力

#define LEFT_ANGLE		90	// 左ブロック方向
#define RIGHT_ANGLE		270 // 右ブロック方向
#define FORWARD_ANGLE	0	// 正面方向

#define ONLINE			40	// 黒線上での輝度値
#define NotONLINE 		120 // 黒線以外での輝度値

#define EDGE_LINE		80	// 黒線との境界線

#define NOT_BLOCK_DISTANCE	8 // ブロックを認識していないときの距離
#define BLOCK_DISTANCE 		4 // ブロックを認識した時のきょり　

void ETSumoStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager* IOManager = InOutManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();

	HSLColorKind DetectColor = IOManager->HSLKind;
	PIDData pid = PIDDataManager::GetInstance()->GetPIDData(BlockMovePIDState);
	int currentAngle = SpManager->RobotAngle;

	switch(CurrentState) {
		// 土俵直進中
		case ForwardArena:
			// 黒線認識したら、黒線横断中状態に遷移
			if(IOManager->InputData.ReflectLight < ONLINE) {
				CurrentState = ForwardOverLeftLine;
				break;
			}
			// 黒線まではラインの左側のエッジに沿ってライントレース
			IOManager->LineTraceAction(pid, EDGE_LINE, true);
			// ライントレース中なので、角度は常にライン方向
			SpManager->ResetAngle(FORWARD_ANGLE);
			break;
		
		// 土俵左黒線横断中
		case ForwardOverLeftLine:
			// 黒線を抜けたら、左ブロック方向旋回状態に遷移
			if(IOManager->InputData.ReflectLight > NotONLINE) {
				CurrentState = TurnLeftPlace;
				break;
			}
			// 黒線認識までは普通に直進
			IOManager->Forward(FSPEED);
			break;
		
		// 左ブロック方向旋回
		case TurnLeftPlace:
			// 黒線上に乗ったら左ブロックへの直進状態に移行
			if(IOManager->InputData.ReflectLight < EDGE_LINE) {
				CurrentState = ForwardLeftPlace;
				break;
			}
			
			// 左ブロック方向を向くまで旋回
			IOManager->Turn(currentAngle, LEFT_ANGLE, TURN_SPEED);
			break;

		// 左ブロックまで直進
		case ForwardLeftPlace:
			// 超音波センサでブロックを認識した場合には、色認識に移行
			if(IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
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
			if(DetectColor == PrevColor && DetectColor != HSLBlack) {
				ColorDetectCount++;
				if(ColorDetectCount > 10) {
					ColorDetectCount = 0;
					// 色認識が完了したため、アームを下降させる
					IOManager->DownARMMotor();
					// フィールドの色と土俵の色が一致した場合、寄り切り
					if(ArenaArray[CurrentArena].LeftPlaceColor == DetectColor) {
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
			if(IOManager->InputData.ReflectLight < EDGE_LINE && 
							abs(currentAngle - RIGHT_ANGLE) < 45) {
				CurrentState = ForwardCenterFromLeft;
				break;
			}

			// 左ブロック方向を向くまで旋回
			IOManager->TurnCW(TURN_SPEED);
			break;
		
		// 左ブロックを押し出し
		case OSHIDASHILeft :

			break;
		
		// 左ブロックから中央ラインに向かって前進
		case ForwardCenterFromLeft :
			// 黒線認識したら、黒線横断中状態に遷移
			if(IOManager->InputData.ReflectLight < ONLINE) {
				CurrentState = ForwardOverCenterLineToRight;
				break;
			}
			// 黒線まではラインの左側のエッジに沿ってライントレース
			IOManager->LineTraceAction(pid, EDGE_LINE, true);
			// ライントレース中なので、角度は常にライン方向
			SpManager->ResetAngle(RIGHT_ANGLE);
			
			break;
		
		// 右ブロックへ向けて中央ラインを横断
		case ForwardOverCenterLineToRight:
			// 黒線を抜けたら、右ブロックまで直進に遷移
			if(IOManager->InputData.ReflectLight > NotONLINE) {
				CurrentState = ForwardRightPlace;
				break;
			}
			// 黒線認識までは普通に直進
			IOManager->Forward(FSPEED);
			break;

			break;
		
		/// 右ブロックまで前進
		case ForwardRightPlace :
			// 超音波センサでブロックを認識した場合には、色認識に移行
			if(IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
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
			if(DetectColor == PrevColor && DetectColor != HSLBlack) {
				ColorDetectCount++;
				if(ColorDetectCount > 10) {
					ColorDetectCount = 0;
					// 色認識が完了したため、アームを下降させる
					IOManager->DownARMMotor();
					// フィールドの色と土俵の色が一致した場合、寄り切り
					if(ArenaArray[CurrentArena].RightPlaceColor == DetectColor) {
						CurrentState = YORIKIRIRight;
					} else {
						// フィールドの色と土俵の色が異なる場合、押し出し
						CurrentState = OSHIDASHIRight;
					}
					break;
				}
			} else {
				PrevColor = DetectColor;
				ColorDetectCount = 0;
			}
			break;

		// 右ブロックを寄り切り
		case YORIKIRIRight:
			// ある程度旋回してから、黒線上に乗ったら右ブロックへの直進状態に移行
			if(IOManager->InputData.ReflectLight < EDGE_LINE && 
							abs(currentAngle - LEFT_ANGLE) < 45) {
				CurrentState = ForwardCenterFromRight;
				IOManager->Stop();
				break;
			}

			// 中央方向を向くまで旋回
			IOManager->TurnCW(TURN_SPEED);
			break;
		
		// 右ブロックを押し出し
		case OSHIDASHIRight:
			break;
		
		// 中央方向へ前進
		case ForwardCenterFromRight:
			// 黒線認識したら、黒線横断中状態に遷移
			if(IOManager->InputData.ReflectLight < ONLINE) {
				CurrentState = ForwardOverCenterLineFromRight;
				break;
			}
	
			// エッジの右側をライントレースしながら中央のラインに向かう
			IOManager->LineTraceAction(pid, EDGE_LINE, false);
		
			// ライントレース中なので方向は強制的にライン沿い
			SpManager->ResetAngle(LEFT_ANGLE);
			break;
		
		case ForwardOverCenterLineFromRight:
			// 黒線を抜けたら、前方方向に旋回状態に遷移
			if(IOManager->InputData.ReflectLight > NotONLINE) {
				CurrentState = TurnForward;
				break;
			}
			// 黒線認識までは普通に直進
			IOManager->Forward(FSPEED);
		
		case TurnForward:
			// 黒線上に乗ったら一枚の土俵攻略が終了
			if(IOManager->InputData.ReflectLight < EDGE_LINE) {
				//  四枚目の土俵でなければ次の土俵の攻略に遷移
				if(CurrentArena <= 4) {
					CurrentArena++;
					CurrentState = ForwardArena;                        
				} else { 
					// 全ての土俵の攻略が終わっていれば、ET相撲戦略を終了
					IOManager->Stop();
				}

				CurrentState = ForwardLeftPlace;
				break;
			}
			
			// 左ブロック方向を向くまで旋回
			IOManager->Turn(currentAngle, LEFT_ANGLE, TURN_SPEED);
		
		default :
		break;
	}
}