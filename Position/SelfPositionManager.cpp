#include <string.h>
#include <math.h>
#include "ev3api.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "ParticleFilter.h"
#include "SelfPositionManager.h"

#define POS_SIGMA 	10// パーティクルの散布範囲
#define ANGLE_SIGMA	0.001// パーティクルの角度範囲


//#define POS_SIGMA 	10// パーティクルの散布範囲
//#define ANGLE_SIGMA	0.001// パーティクルの角度範囲


SelfPositionManager* SelfPositionManager::GetInstance(){
	static SelfPositionManager manager;
	return &manager;
}

void SelfPositionManager::SendData()
{
	// 送信処理	
	SelfPositionData p;
	p.PositionX = (uint)RobotPoint.X;
	p.PositionY = (uint)RobotPoint.Y;
	p.Angle = (ushort)RobotAngle;
	p.Distance = (uint)Distance;

	// 自己位置情報と同様のサイズを持つ配列にコピー
	memcpy(buff_self_position, &p, sizeof(SelfPositionData));
}

void SelfPositionManager::UpdatePosition(int8_t leftMotorCount, int8_t rightMotorCount)
{
	// モータが一切動いていない場合には、パーティクルフィルタも駆動しない
	if(leftMotorCount == 0 && rightMotorCount == 0)	{
		SendData();
		return;
	}	

	// パーティクルフィルタONなら、フィルタに対してリサンプリングを実施する
	if (ParticleFilterON) {
		Filter->Resampling(&RobotPoint,RobotAngle, POS_SIGMA, ANGLE_SIGMA);
	} else {
		// OFFなら、標準偏差0でリサンプリングを行う
		Filter->Resampling(&RobotPoint,RobotAngle, 0, 0);
	}

	// パーティクルフィルタの状態遷移を実施
	Filter->UpdateParticle(leftMotorCount, rightMotorCount);

	// パーティクルフィルタの尤度計算と位置確定
	Filter->Localize();

	// パーティクルフィルタからフィルタ透過後の位置を取得して、自己位置とする
	RobotPoint.X = Filter->RobotPoint.X;
	RobotPoint.Y = Filter->RobotPoint.Y;
	RobotAngle = Filter->RobotAngle;
	
	// 先頭の粒子の移動量を、自身の移動量に加算
	Distance += Filter->Distance;

	// 送信処理	
	SendData();
}

// 自己位置情報電文の内容で自身の情報をリセットする
void SelfPositionManager::ResetPosition(SelfPositionData p)
{
	RobotPoint.X = p.PositionX;
	RobotPoint.Y = p.PositionY;
	RobotAngle = p.Angle;
	Distance = p.Distance;

	// パーティクルフィルタを初期化
	if(Filter != NULL) {
		delete Filter;
	}
	
	Filter = new ParticleFilter();
}

// 位置を強制的にリセットする
void SelfPositionManager::ResetPoint(Point* p)
{
	RobotPoint.X = p->X;
	RobotPoint.Y = p->Y;

	// 送信処理	
	SendData();
}

void SelfPositionManager::ResetX(double X)
{
	RobotPoint.X = X;

	// 送信処理	
	SendData();
}

void SelfPositionManager::ResetY(double Y)
{
	RobotPoint.Y = Y;

	// 送信処理	
	SendData();
}

// 角度を強制的にリセットする
void SelfPositionManager::ResetAngle(double currentAngle)
{
	RobotAngle = currentAngle;

	// 送信処理	
	SendData();
}

// ある座標との距離を求める
double SelfPositionManager::GetDistance(Point* p)
{
	return hypot(p->X - RobotPoint.X, p->Y - RobotPoint.Y);
}