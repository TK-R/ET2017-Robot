#include <string.h>
#include <math.h>
#include "ev3api.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "SelfPositionManager.h"

SelfPositionManager* SelfPositionManager::GetInstance(){
	static SelfPositionManager manager;
	return &manager;
}

void SelfPositionManager::UpdatePosition(int8_t leftMotorCount, int8_t rightMotorCount)
{
	// 車輪直径(mm)
	double TireDiameter = 81.6;

	// 左右車輪間の距離(mm)
	double d = 128;	 
	
	// 左車輪の移動距離
	double Tl = TireDiameter * M_PI * leftMotorCount / 360.0; 
	
	// 右車輪の移動距離
	double Tr = TireDiameter * M_PI * rightMotorCount / 360.0; 

	// モータエンコーダ値の変化量
	double D = (Tl + Tr) / 2.0;
	
	// 角度の変化量	
	double theta = (Tr - Tl) / d; 
	// 
	// 計算用にラジアンに変換した前回までの角度
	double theta0 = Angle * M_PI / 180.0;

	double X = (D * cos(theta0 + (theta / 2.0)));
	double Y = (D * sin(theta0 + (theta / 2.0)));
	 
	PositionX += X;
	PositionY -= Y;
	Distance += D;
	
	// 0 - 360度の範囲に変更
	if(Angle + (theta / M_PI * 180.0) > 360) {
		Angle = Angle + (theta / M_PI * 180.0) - 360;
	}else if(Angle + (theta / M_PI * 180.0) < 0) {
		Angle = 360 - Angle +(theta / M_PI * 180.0);
	}else {
		Angle += (theta / M_PI * 180.0);
	}
	
	SelfPositionData p;
	p.PositionX = (uint)PositionX;
	p.PositionY = (uint)PositionY;
	p.Angle = (ushort)Angle;
	p.Distance = (uint)Distance;

	// 自己位置情報と同様のサイズを持つ配列にコピー
	memcpy(buff_self_position, &p, sizeof(SelfPositionData));
}

void SelfPositionManager::ResetPosition(SelfPositionData p)
{
	PositionX = p.PositionX;
	PositionY = p.PositionY;
	Angle = p.Angle;
	Distance = p.Distance;
}

// ある座標との距離を求める
double SelfPositionManager::GetDistance(Point* p)
{
	return hypot(p->X - PositionX, p->Y - PositionY);
}