#include <stdlib.h>
#include <math.h>

#include "ev3api.h"
#include "Point.h"
#include "FieldMap.h"
#include "InOutManager.h"
#include "ParticleFilter.h"


const double sintable[256]={
	0.000000000000,0.006159946638,0.012319659535,0.018478904959,0.024637449195,0.030795058556,0.036951499389,0.043106538086,0.049259941093,0.055411474916,
	0.061560906134,0.067708001405,0.073852527475,0.079994251189,0.086132939496,0.092268359463,0.098400278280,0.104528463268,0.110652681892,0.116772701766,
	0.122888290665,0.128999216530,0.135105247481,0.141206151823,0.147301698055,0.153391654879,0.159475791210,0.165553876184,0.171625679166,0.177690969760,
	0.183749517817,0.189801093442,0.195845467007,0.201882409157,0.207911690818,0.213933083206,0.219946357840,0.225951286542,0.231947641454,0.237935195043,
	0.243913720108,0.249882989794,0.255842777594,0.261792857363,0.267733003322,0.273662990072,0.279582592597,0.285491586275,0.291389746889,0.297276850631,
	0.303152674113,0.309016994375,0.314869588894,0.320710235590,0.326538712840,0.332354799480,0.338158274816,0.343948918634,0.349726511206,0.355490833300,
	0.361241666187,0.366978791650,0.372701991991,0.378411050042,0.384105749172,0.389785873293,0.395451206871,0.401101534933,0.406736643076,0.412356317474,
	0.417960344887,0.423548512668,0.429120608773,0.434676421766,0.440215740831,0.445738355777,0.451244057045,0.456732635722,0.462203883540,0.467657592893,
	0.473093556836,0.478511569101,0.483911424100,0.489292916934,0.494655843400,0.500000000000,0.505325183949,0.510631193181,0.515917826358,0.521184882877,
	0.526432162877,0.531659467250,0.536866597644,0.542053356472,0.547219546922,0.552364972961,0.557489439343,0.562592751620,0.567674716145,0.572735140081,
	0.577773831408,0.582790598933,0.587785252292,0.592757601963,0.597707459266,0.602634636379,0.607538946339,0.612420203049,0.617278221290,0.622112816721,
	0.626923805894,0.631711006253,0.636474236147,0.641213314834,0.645928062487,0.650618300204,0.655283850013,0.659924534879,0.664540178708,0.669130606359,
	0.673695643647,0.678235117349,0.682748855215,0.687236685969,0.691698439319,0.696133945963,0.700543037593,0.704925546906,0.709281307606,0.713610154412,
	0.717911923064,0.722186450332,0.726433574016,0.730653132959,0.734844967047,0.739008917221,0.743144825477,0.747252534879,0.751331889557,0.755382734719,
	0.759404916655,0.763398282741,0.767362681449,0.771297962347,0.775203976111,0.779080574526,0.782927610492,0.786744938033,0.790532412300,0.794289889575,
	0.798017227280,0.801714283980,0.805380919389,0.809016994375,0.812622370966,0.816196912356,0.819740482907,0.823252948158,0.826734174826,0.830184030816,
	0.833602385221,0.836989108332,0.840344071638,0.843667147834,0.846958210824,0.850217135730,0.853443798888,0.856638077864,0.859799851448,0.862928999667,
	0.866025403784,0.869088946306,0.872119510984,0.875116982823,0.878081248084,0.881012194286,0.883909710214,0.886773685920,0.889604012731,0.892400583248,
	0.895163291355,0.897892032220,0.900586702301,0.903247199346,0.905873422403,0.908465271820,0.911022649246,0.913545457643,0.916033601280,0.918486985746,
	0.920905517945,0.923289106105,0.925637659782,0.927951089857,0.930229308547,0.932472229404,0.934679767321,0.936851838531,0.938988360615,0.941089252501,
	0.943154434471,0.945183828161,0.947177356564,0.949134944036,0.951056516295,0.952942000427,0.954791324887,0.956604419500,0.958381215470,0.960121645375,
	0.961825643173,0.963493144206,0.965124085200,0.966718404269,0.968276040916,0.969796936035,0.971281031916,0.972728272245,0.974138602105,0.975511967980,
	0.976848317760,0.978147600734,0.979409767601,0.980634770469,0.981822562854,0.982973099684,0.984086337303,0.985162233468,0.986200747353,0.987201839554,
	0.988165472081,0.989091608371,0.989980213281,0.990831253092,0.991644695511,0.992420509672,0.993158666137,0.993859136895,0.994521895368,0.995146916407,
	0.995734176295,0.996283652748,0.996795324917,0.997269173386,0.997705180174,0.998103328737,0.998463603967,0.998785992194,0.999070481184,0.999317060143,
	0.999525719713,0.999696451978,0.999829250458,0.999924110115,0.999981027349,1.000000000000
};

// パーティクルフィルタで使用するテーブル型sin
double mysin(double x) {
	int sinindex;
	int cosindex;
	int cossgn=0;
	int sinsgn=0;
	double gosa;
	double s,c;
	if(x>0)sinindex=(int)(x*510/M_PI+0.5);//四捨五入を行っておく．
	else   sinindex=(int)(x*510/M_PI-0.5);//単なる切捨てだと精度が半分になってしまう．
	cosindex=sinindex+255;
	gosa=x-sinindex*M_PI/510;
	if(sinindex<0){sinindex=-sinindex;sinsgn=1;}
	if(cosindex<0){cosindex=-cosindex;cossgn=1;}
	sinindex%=1020;//255*4,複数回回転を除去
	cosindex%=1020;//255*4,複数回回転を除去
	if(sinindex>510){sinindex=1020-sinindex;sinsgn=1-sinsgn;}//定義域を第一，第二象限に制限
	if(cosindex>510)cosindex=1020-cosindex;//定義域を第一，第二象限に制限
	if(sinindex>255)sinindex=510-sinindex;//定義域を第一象限に制限
	if(cosindex>255){cosindex=510-cosindex;cossgn=1-cossgn;}//定義域を第一象限に制限

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
//	double theta = (Tr - Tl) / d; 
	double theta = (Tr - Tl) / d * (171.0 / 180.0); 

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

