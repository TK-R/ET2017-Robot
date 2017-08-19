#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "ETSumoStrategy.h"

#define FSPEED  		15	// 直進時の出力
#define TURN_SPEED		15	// 旋回時の出力

#define LEFT_ANGLE		90	// 左ブロック方向
#define RIGHT_ANGLE		270 // 右ブロック方向
#define FORWARD_ANGLE	0	// 正面方向

#define ONLINE			40	// 黒線上での輝度値
#define NotONLINE 		120 // 黒線以外での輝度値

void ETSumoStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager* IOManager = InOutManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();

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
			if(IOManager->InputData.ReflectLight < ONLINE) {
				CurrentState = TurnLeftPlace;
				break;
			}
			// 黒線認識までは普通に直進
			IOManager->Forward(FSPEED);
			break;
		
		case TurnLeftPlace:
			// 左ブロック方向を向いたら左ブロックまで直進に遷移
			if(currentAngle == LEFT_ANGLE) {
				CurrentState = ForwardLeftPlace;
				break;
			}
			
			// 左ブロック方向を向くまで旋回
			IOManager->Turn(currentAngle, LEFT_ANGLE, TURN_SPEED);
			break;
		case ForwardLeftPlace:
			
			// 一定距離直進するまで直進
			IOManager->LineTraceAction(false);
		default :
		break;
	}
}