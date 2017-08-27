#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "PrizeStrategy.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "SerialData.h"
#include "PIDDataManager.h"

#define OSHIDASHI_SPEED 20 // 押し出し動作時のスピード

#define FIRST_TURN_SPEED 25 // 高速旋回時の出力
#define TURN_SPEED 15 // 旋回時の出力

#define LEFT_ANGLE 90   // 左ブロック方向
#define RIGHT_ANGLE 270 // 右ブロック方向
#define FORWARD_ANGLE 1 // 正面方向

#define ONLINE 25	  // 黒線上での輝度値
#define NotONLINE 60  // 黒線以外での輝度値
#define EDGE_LINE 120 // 黒線との境界線
#define FIELD	  180 // 

#define NOT_BLOCK_DISTANCE 8 // ブロックを認識していないときの距離
#define BLOCK_DISTANCE 3	 // ブロックを認識した時のきょり　

void PrizeStrategy::Run()
{
	// 使用するシングルトンクラスのインスタンスを取得
	InOutManager *IOManager = InOutManager::GetInstance();
	SelfPositionManager *SpManager = SelfPositionManager::GetInstance();
	HSLColorKind DetectColor = IOManager->HSLKind;
	
	SpManager->ParticleFilterON = false;
ACTION :

	PIDData pid = PIDDataManager::GetInstance()->GetPIDData(ETSumoPIDState);
	int currentAngle = SpManager->RobotAngle;
	Point currentPoint = SpManager->RobotPoint;

	switch (CurrentState) {
	default:
		break;
	}
}