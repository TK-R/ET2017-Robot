#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "ETSumoStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"

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

	int currentAngle = SpManager->RobotAngle;

	switch(CurrentState) {
		// 土俵直進中
		case ForwardArena:
			// 黒線認識したら、黒線横断中状態に遷移
			if(IOManager->InputData.ReflectLight < ONLINE) {
				CurrentState = ForwardOverLine;
				break;
			}
			// 黒線認識までは普通に直進
			IOManager->Forward(FSPEED);
			break;
		
			// 土俵直進にて黒線横断中
		case ForwardOverLine:
			// 黒線を抜けたら、左ブロック方向旋回状態に遷移
			if(IOManager->InputData.ReflectLight > NotONLINE) {
				CurrentState = TurnLeftPlace;
				break;
			}
			// 黒線認識までは普通に直進
			IOManager->Forward(FSPEED);
			break;
		
		case TurnLeftPlace:
			// 黒線上に乗ったら左ブロックへの直進状態に移行
			if(IOManager->InputData.ReflectLight < EDGE_LINE) {
				CurrentState = ForwardLeftPlace;
				break;
			}
			
			// 左ブロック方向を向くまで旋回
			IOManager->Turn(currentAngle, LEFT_ANGLE, TURN_SPEED);
			break;
		case ForwardLeftPlace:
			// 超音波センサでブロックを認識した場合には、色認識に移行
			if(IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
				CurrentState = DetectLeftBlock;
				IOManager->Stop();
				break;
			}
			
			IOManager->Forward(FSPEED);
			break;
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
		case YORIKIRILeft:
			// ある程度旋回してから、黒線上に乗ったら右ブロックへの直進状態に移行
			if(IOManager->InputData.ReflectLight < EDGE_LINE && 
							abs(currentAngle - RIGHT_ANGLE) < 45) {
				CurrentState = ForwardRightPlace;
				break;
			}

			// 左ブロック方向を向くまで旋回
			IOManager->TurnCW(TURN_SPEED);
			break;
		case ForwardRightPlace :
			IOManager->Forward(FSPEED);
			
			// 超音波センサでブロックを認識した場合には、色認識に移行
			if(IOManager->InputData.SonarDistance <= BLOCK_DISTANCE) {
				CurrentState = DetectRightBlock;
				IOManager->Stop();
				break;
			}
			break;

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

		case YORIKIRIRight:
			// ある程度旋回してから、黒線上に乗ったら右ブロックへの直進状態に移行
			if(IOManager->InputData.ReflectLight < EDGE_LINE && 
							abs(currentAngle - LEFT_ANGLE) < 45) {
				CurrentState = ForwardCenter;
				IOManager->Stop();
				break;
			}

			// 中央方向を向くまで旋回
			IOManager->TurnCW(TURN_SPEED);
			break;

		default :
		break;
	}
}