#include <stdlib.h>
#include <math.h>

#include "ev3api.h"
#include "Point.h"
#include "FieldMap.h"
#include "InOutManager.h"
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

	// ラジアンに変換した前回の角度
	double theta0 = RobotAngle * M_PI / 180.0;

	// 0 - 360度の範囲に変更
	if(RobotAngle + (theta / M_PI * 180.0) > 360) {
		RobotAngle = RobotAngle + (theta / M_PI * 180.0) - 360;
	}else if(RobotAngle + (theta / M_PI * 180.0) < 0) {
		RobotAngle = 360 + RobotAngle +(theta / M_PI * 180.0);
	}else {
		RobotAngle += (theta / M_PI * 180.0);
	}

	// 変化量を、正規分布の値を加味して算出
	// double sinc_theta = fabs(theta/2.0)<1.0e-10 ? 1.0 : sin(theta/2.0)/(theta/2.0);
	// double deltaX = D * cos(theta0 + (theta / 2.0)) * sinc_theta;
	// double deltaY = D * sin(theta0 + (theta / 2.0)) * sinc_theta;
	double deltaX = D * cos(theta0 + (theta / 2.0));
	double deltaY = D * sin(theta0 + (theta / 2.0));
		
	RobotPoint.X += deltaX;
	RobotPoint.Y -= deltaY;

	// 地図上の色情報を取得
	HSLColor mapColor = FieldMap::GetInstance()->GetHSLColor(RobotPoint.X, RobotPoint.Y);

	// センサから得た色情報を元に、尤度を定義
	Likelihood = ColorDecision::GetLikelihoodLuminosity(mapColor.Luminosity, InOutManager::GetInstance()->HSLValue.Luminosity);	
}

// 指定した座標で粒子を撒きなおす
void Particle::Reset(Point* newPoint, double newAngle, double pointSigma, double angleSigma)
{
	// 正規分布の値を加味する
	RobotPoint.X = newPoint->X + NormalDistribution(pointSigma);
	RobotPoint.Y = newPoint->Y + NormalDistribution(pointSigma);
	RobotAngle = newAngle + NormalDistribution(angleSigma);

	Likelihood = 0;
}

// 中心値を指定して、パーティクルを散布する
void ParticleFilter::Resampling(Point* newPoint, double newAngle, double pointSigma, double angleSigma)
{
	// 全ての粒子に対しt、中心座標を指定した再散布を行う
	for(Particle* p: ParticleArray) {
		p->Reset(newPoint, newAngle, pointSigma, angleSigma);
	}
}

// パーティクルに対して、座標のアップデートを行う
void ParticleFilter::UpdateParticle(int8_t leftMotorCount, int8_t rightMotorCount)
{
	// すべての粒子に対して状態遷移を実施する
	for(Particle*  p: ParticleArray) {
		p->Update(leftMotorCount,  rightMotorCount);
	}

}

// パーティクルのうち、尤度の高い物の平均値を元に、
// 自身の状態を更新する
void ParticleFilter::Localize()
{	
	// 尤度が閾値より高い粒子は、すべて平均化の対象とする。
	const double min = 0.1;

	std::vector<Particle*> pVector;

	for(Particle* p : ParticleArray) {
		if(p->Likelihood > min) pVector.push_back(p);
	}

	double x = 0, y = 0, angle = 0;

	// 尤度の高い粒子が存在しない場合には、すべての粒子を平均化
	if(pVector.size() == 0)	{
		for(Particle* p  : ParticleArray) {
			x += p->RobotPoint.X;
			y += p->RobotPoint.Y;
			angle += p->RobotAngle;
		}

		x /= PARTICLE_COUNT;
		y /= PARTICLE_COUNT;
		angle /= PARTICLE_COUNT;
	} else {
		// 閾値以上の粒子を平均化
		for(Particle* p : pVector) {
			x += p->RobotPoint.X;
			y += p->RobotPoint.Y;
			angle += p->RobotAngle;
		}

		x /= pVector.size();
		y /= pVector.size();
		angle /= pVector.size();
	}

	// atan2の出力範囲が-pi ~ piのため、0-360の範囲に修正
	if(angle < 0) {
		angle = 360 + angle;
	}

	// 算出した平均値でフィールドを更新
	RobotPoint.X = x;
	RobotPoint.Y = y;
	RobotAngle = angle;
}
