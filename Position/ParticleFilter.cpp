#include <stdlib.h>
#include <math.h>

#include "ev3api.h"
#include "Point.h"
#include "FieldMap.h"
#include "InOutManager.h"
#include "ParticleFilter.h"


const double sintable[128]={
	0.000000,0.012368,0.024734,0.037097,0.049454,0.061803,0.074143,0.086471,0.098786,0.111087,
	0.123370,0.135634,0.147877,0.160098,0.172295,0.184465,0.196606,0.208718,0.220798,0.232844,
	0.244854,0.256827,0.268761,0.280654,0.292503,0.304308,0.316066,0.327776,0.339436,0.351044,
	0.362598,0.374097,0.385538,0.396921,0.408243,0.419502,0.430697,0.441826,0.452888,0.463881,
	0.474802,0.485651,0.496425,0.507124,0.517745,0.528287,0.538748,0.549126,0.559421,0.569630,
	0.579752,0.589785,0.599728,0.609579,0.619337,0.629000,0.638567,0.648036,0.657406,0.666676,
	0.675843,0.684908,0.693867,0.702720,0.711466,0.720103,0.728630,0.737045,0.745348,0.753536,
	0.761610,0.769566,0.777405,0.785125,0.792725,0.800204,0.807561,0.814793,0.821901,0.828884,
	0.835739,0.842467,0.849066,0.855535,0.861873,0.868079,0.874153,0.880093,0.885898,0.891567,
	0.897101,0.902497,0.907754,0.912873,0.917853,0.922692,0.927390,0.931946,0.936359,0.940629,
	0.944755,0.948737,0.952574,0.956265,0.959809,0.963207,0.966457,0.969560,0.972514,0.975320,
	0.977976,0.980482,0.982839,0.985045,0.987101,0.989006,0.990759,0.992361,0.993811,0.995109,
	0.996254,0.997248,0.998088,0.998776,0.999312,0.999694,0.999924,1.000000
};

// パーティクルフィルタで使用するテーブル型sin
double mysin(double x) {
	int sinindex;
	int cosindex;
	int cossgn=0;
	int sinsgn=0;
	double gosa;
	double s,c;
	if(x>0)sinindex=(int)(x*254/M_PI+0.5);//四捨五入を行っておく．
	else   sinindex=(int)(x*254/M_PI-0.5);//単なる切捨てだと精度が半分になってしまう．
	cosindex=sinindex+127;
	gosa=x-sinindex*M_PI/254;
	if(sinindex<0){sinindex=-sinindex;sinsgn=1;}
	if(cosindex<0){cosindex=-cosindex;cossgn=1;}
	sinindex%=508;//127*4,複数回回転を除去
	cosindex%=508;//127*4,複数回回転を除去
	if(sinindex>254){sinindex=508-sinindex;sinsgn=1-sinsgn;}//定義域を第一，第二象限に制限
	if(cosindex>254){cosindex=508-cosindex;cossgn=1-cossgn;}//定義域を第一，第二象限に制限
	if(sinindex>127)sinindex=254-sinindex;//定義域を第一象限に制限
	if(cosindex>127)cosindex=254-cosindex;//定義域を第一象限に制限

	if(sinsgn)s=-sintable[sinindex];
	else      s= sintable[sinindex];
	if(cossgn)c=-sintable[cosindex];
	else      c= sintable[cosindex];
	return s+gosa*c;	
}

// パーティクルフィルタで使用するテーブル型cos
double mycos(double x) {
	return mysin(x+M_PI/2);
}


// 正規分布でsigmaに指定した範囲内の値を取り出す
double Particle::NormalDistribution(double sigma) {
	double alpha = (double)rand()/RAND_MAX;
    double beta = (double)rand()/RAND_MAX * M_PI * 2;
    double tmp = mysin(beta);
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
	Distance = (Tl + Tr) / 2.0;
	
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
	double sinc = 1 - theta*theta/24.0 * (1 - theta*theta/80.0 * (1 - theta*theta/168.0));	//sinc(theta/2)を3次までテイラー展開
	// double sinc_theta = fabs(theta/2.0)<1.0e-10 ? 1.0 : sin(theta/2.0)/(theta/2.0);
	double deltaX = Distance * mycos(theta0 + (theta / 2.0)) * sinc;
	double deltaY = Distance * mysin(theta0 + (theta / 2.0)) * sinc;
	// double deltaX = D * cos(theta0 + (theta / 2.0));
	// double deltaY = D * sin(theta0 + (theta / 2.0));
		
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

	double x = 0, y = 0, angleX = 0, angleY = 0, angle = 0;

	// 尤度の高い粒子が存在しない場合には、すべての粒子を平均化
	if(pVector.size() == 0)	{
		for(Particle* p  : ParticleArray) {
			x += p->RobotPoint.X;
			y += p->RobotPoint.Y;
			angleX += mycos(p->RobotAngle * M_PI / 180);  
			angleY += mysin(p->RobotAngle * M_PI / 180); 
		}

		x /= PARTICLE_COUNT;
		y /= PARTICLE_COUNT;
		angle = atan2(angleY / PARTICLE_COUNT, angleX / PARTICLE_COUNT) * 180 / M_PI; 

	} else {
		// 閾値以上の粒子を平均化
		for(Particle* p : pVector) {
			x += p->RobotPoint.X;
			y += p->RobotPoint.Y;
			angleX += mycos(p->RobotAngle * M_PI / 180);  
			angleY += mysin(p->RobotAngle * M_PI / 180); 
		}

		x /= pVector.size();
		y /= pVector.size();
		angle = atan2(angleY / pVector.size() ,angleX / pVector.size()) * 180 / M_PI; 
	}

	// atan2の出力範囲が-pi ~ piのため、0-360の範囲に修正
	if(angle < 0) {
		angle = 360 + angle;
	}

	// 算出した平均値でフィールドを更新
	RobotPoint.X = x;
	RobotPoint.Y = y;
	RobotAngle = angle;
	Distance = ParticleArray[0]->Distance;
}

