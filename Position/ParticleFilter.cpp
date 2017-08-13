#include <stdlib.h>
#include <math.h>

#include "ev3api.h"
#include "Point.h"
#include "ParticleFilter.h"

// 正規分布でsigmaに指定した範囲内の値を取り出す
double Particle::NormalDistribution(double sigma) {
	double alpha = (double)rand()/RAND_MAX;
    double beta = (double)rand()/RAND_MAX * M_PI * 2;
    double tmp = sin(beta);
    return sigma * sqrt(-2 * log(alpha)) * tmp;
}


// 座標を更新して、尤度を再計算する
void Particle::Update(int8_t leftMotorCount, int8_t rightMotorCount)
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

	// 0 - 360度の範囲に変更
	if(Angle + (theta / M_PI * 180.0) > 360) {
		Angle = Angle + (theta / M_PI * 180.0) - 360;
	}else if(Angle + (theta / M_PI * 180.0) < 0) {
		Angle = 360 - Angle +(theta / M_PI * 180.0);
	}else {
		Angle += (theta / M_PI * 180.0);
	}

	// ラジアンに変換した前回の角度
	double theta0 = Angle * M_PI / 180.0;

	// 変化量を、正規分布の値を加味して算出
	double deltaX = (D * cos(theta0 + (theta / 2.0)));
	double deltaY = (D * sin(theta0 + (theta / 2.0)));
		
	RobotPoint.X += deltaX;
	RobotPoint.Y -= deltaY;

	// TODO 尤度を計算する

}

// 指定した座標で粒子を撒きなおす
void Particle::Reset(Point* newPoint, double newAngle, double sigma)
{
	// 正規分布の値を加味する
	RobotPoint.X = newPoint->X + NormalDistribution(sigma);
	RobotPoint.Y = newPoint->Y + NormalDistribution(sigma);
	Angle = newAngle + NormalDistribution(sigma);

	Likelihood = 0;
}

// 中心値を指定して、パーティクルを散布する
void ParticleFilter::Resampling(Point* newPoint, double Angle, double sigma)
{

}

// パーティクルに対して、座標のアップデートを行う
void ParticleFilter::UpdateParticle(int8_t leftMotorCount, int8_t rightMotorCount)
{

}

// パーティクルのうち、尤度の高い物の平均値をとって、次の中心座標を決定する。
void ParticleFilter::Localize()
{
	return;
}
