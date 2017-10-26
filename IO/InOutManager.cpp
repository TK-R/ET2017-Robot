#include "ev3api.h"
#include <string.h>
#include "app.h"
#include "SerialData.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "InOutManager.h"
#include "SerialSendTask.h"
#include "PIDDataManager.h"

// 加速制御における0-100到達時間（ms）
#define ACCEL_RATE 1000

// 減速制御における0-100到達時間（ms）
#define BRAKE_RATE 250

// 加減速制御における制御開始速度（%)
#define ACCEL_START_POWER 50

InOutManager::InOutManager()
{
	RightMotor = new Motor(PORT_A, false, LARGE_MOTOR);
	LeftMotor = new Motor(PORT_B, false, LARGE_MOTOR);	
	ArmMotor = new Motor(PORT_C, false, LARGE_MOTOR);
	TailMotor = new Motor(PORT_D, false, MEDIUM_MOTOR);

	// アーム位置の初期化を行う
	ev3_motor_set_power(EV3_PORT_C, -15);	
	Color = new ColorSensor(PORT_3);

	ArmMotor->setCount(0);
	ev3_motor_rotate(EV3_PORT_C, 35, 15, true);
	ev3_motor_stop(EV3_PORT_C, true);
	ArmMotor->setCount(0);

	ev3_motor_stop(EV3_PORT_D, true);
	TailMotor->setCount(0);

	Touch = new TouchSensor(PORT_1);
	Sonar = new SonarSensor(PORT_2);
	Gyro = new GyroSensor(PORT_4);
}

InOutManager* InOutManager::GetInstance()
{
	static InOutManager manager; 
	return &manager;
}

// 前進するように左右モータの値を更新する
void InOutManager::Forward(int power)
{
	int diff = InputData.RightMotorAngle - InputData.LeftMotorAngle;
	auto pid = PIDDataManager::GetInstance()->GetPIDData(ForwardPID);
	
	// 積分時間 = 1s
	uint IntegralCount = 50;

	// 積分処理
	IntegralForwardDiff.push_back(diff);
	if(IntegralForwardDiff.size() > IntegralCount) IntegralForwardDiff.erase(IntegralForwardDiff.begin());

	// 積分偏差
	double intDiff = 0.0;
	for(int d : IntegralForwardDiff)
		intDiff  += d;
 	intDiff /= IntegralForwardDiff.size();	

	if(IntegralForwardDiff.size() > 5) {
		Forward(power, diff * 30 * pid.PGain + (diff- PrevForwardDiff) * pid.DGain * 30 + intDiff * pid.IGain * 60);		
	} else {
		Forward(power, 0);
	}

	PrevForwardDiff = diff;
}

// 出力値と操舵角から左右モータの値を更新する（0：直進 100：右方向へ -100：左方向へ）
void InOutManager::Forward(int power, int steering)
{	
	Steering = steering;


	// ステアリング値は0-100の範囲内
	if(steering > 100) Steering = 100;
	if(steering < -100) Steering = -100;

	// 右側へ
	if (steering > 0)
	{
		OutputData.LeftMotorPower = (int8_t)power;
		OutputData.RightMotorPower = (int8_t)(power * (100.0 - Steering) / 100.0);
	}
	// 左側へ
	else
	{
		OutputData.LeftMotorPower = (int8_t)(power * (100.0 + Steering) / 100.0);
		OutputData.RightMotorPower = (int8_t)power;
	}
}


// 後退するように左右モータの値を更新する
void InOutManager::Back(int power)
{
	OutputData.LeftMotorPower = -1 * power;
	OutputData.RightMotorPower = -1 * power;
}

// 移動用モータを停止する
void InOutManager::Stop()
{
	AccelPower = 0;

	OutputData.LeftMotorPower = 0;
	OutputData.RightMotorPower = 0;
}

// 時計周りすべきか判定して、時計回りならtrueを返す
bool InOutManager::JudgeTurnCW(int currentAngle, int targetAngle)
{

	if(currentAngle < 180) {
		// 回転角度が180度を必ず越えない場合
		if(currentAngle > targetAngle) {
			return true;
		} else if(targetAngle - currentAngle < 180) {
			return false;
		} else {
			return true;
		}
	} else {
		// 回転角度が180度を必ず越えない場合
		if(targetAngle > currentAngle) {
			return false;
		} else if(currentAngle - targetAngle < 180) {
			return true;
		} else {
			return false;
		}
	}
}


void InOutManager::TurnWithBlock(bool CW, int power, double ratio)
{
	double ohterPower = ratio * power;

	if(CW) {
		OutputData.LeftMotorPower = power;
		OutputData.RightMotorPower = ohterPower;
	} else {
		OutputData.LeftMotorPower = ohterPower;
		OutputData.RightMotorPower = power;
	} 
}

void InOutManager::Turn(bool CW, int power)
{
	double ohterPower = -1 * power;
	
	if(CW) {
		OutputData.LeftMotorPower = power;
		OutputData.RightMotorPower = ohterPower;
	} else {
		OutputData.LeftMotorPower = ohterPower;
		OutputData.RightMotorPower = power;
	}
}

void InOutManager::Turn(int currentAngle, int targetAngle, int power)
{
	if(abs(currentAngle-targetAngle) < 20) power = 25;

	if(currentAngle < 180) {
		// 回転角度が180度を必ず越えない場合
		if(currentAngle > targetAngle) {
			TurnCW(power);
		} else if(targetAngle - currentAngle < 180) {
			TurnCCW(power);
		} else {
			TurnCW(power);
		}
	} else {
		// 回転角度が180度を必ず越えない場合
		if(targetAngle > currentAngle) {
			TurnCCW(power);
		} else if(currentAngle - targetAngle < 180) {
			TurnCW(power);
		} else {
			TurnCCW(power);
		}
	}
}

// 時計回りに回転するように左右モータの値を更新する
void InOutManager::TurnCW(int power)
{
	OutputData.LeftMotorPower = power;
	OutputData.RightMotorPower = -1 * power;
}

// 反時計回りに回転するように左右モータ値を更新する
void InOutManager::TurnCCW(int power)
{
	OutputData.LeftMotorPower = -1 * power;
	OutputData.RightMotorPower = power;
}

void InOutManager::LineTraceClear(int Center)
{
	PrevDiff = Center;
	IntegralDiff.clear();
	IntegralDiff.push_back(Center);
}

// ライントレースを実施するように左右モータ値を更新する
void InOutManager::LineTraceAction(PIDData data, int center, bool LeftEdge)
{
	int light = InputData.ReflectLight;

	int diff = LeftEdge ? (int)light - center :  (int)center - light;

	uint IntegralCount = 5;
	// 積分処理
	IntegralDiff.push_back(diff);
	if(IntegralDiff.size() > IntegralCount) IntegralDiff.erase(IntegralDiff.begin());

	// 積分偏差
	double intDiff = 0.0;
	for(int d : IntegralDiff)
		intDiff  += d;
 	intDiff /= IntegralDiff.size();	

	int steering = data.PGain * diff + data.IGain * intDiff + (diff - PrevDiff) * data.DGain;
	PrevDiff = diff;

	if(data.BasePower > AccelPower) {
		// 制御開始速度以下なら、開始速度まで一気に引き上げる
		if(data.BasePower > ACCEL_START_POWER && AccelPower < ACCEL_START_POWER) {
			AccelPower =  ACCEL_START_POWER;
		} 

		// 加速制御
		double p = AccelPower + BASE_TIME * 100.0 / ACCEL_RATE;

		// 計算結果と目標値のうち、低いほうを出力値とする
		AccelPower = p > data.BasePower ? data.BasePower : p;
	} else {
		// 制御開始速度以下なら、目標速度まで一気に下げる
		if(AccelPower < ACCEL_START_POWER) {
			AccelPower =  data.BasePower;
		} 

		// 減速制御
		double p = AccelPower - BASE_TIME * 100.0 / BRAKE_RATE;

		// 計算結果と目標値のうち、高いほうを出力値とする
		AccelPower = p < data.BasePower ? data.BasePower : p;
		AccelPower = data.BasePower;
	}

	Forward(AccelPower, steering + data.Steering);

	// 単純直進時のPID値をクリア
	PrevForwardDiff = 0;
	IntegralForwardDiff.clear();
}

// 広報にライントレースを実施するように、左右モータ値を更新する
void InOutManager::BackLineTraceAction(PIDData data, int center, bool LeftEdge)
{
	int light = InputData.ReflectLight;

	int diff = LeftEdge ? (int)light - center :  (int)center - light;
	int steering = data.PGain * diff + (diff - PrevDiff) * data.IGain;
	
	Forward(data.BasePower * -1, steering);

}

void InOutManager::TopARMMotor()
{
	if(ArmState == ArmStateTop) return;

	if(ArmState == ArmStateUp) {
		ev3_motor_rotate(EV3_PORT_C, 10, 100, false);		
	} else if(ArmState == ArmStateDown) {
		ev3_motor_rotate(EV3_PORT_C, 60, 100, false);		
	} else {
		ev3_motor_rotate(EV3_PORT_C, 85, 100, false);		
	} 
	
	ArmState = ArmStateTop;
}

void InOutManager::UpARMMotor()
{
	// 既に上昇済なら何もしない
	if(ArmState == ArmStateUp) return;

	if(ArmState == ArmStateTop){ 
		ev3_motor_rotate(EV3_PORT_C, -10, 100, true);		
	} else if(ArmState == ArmStateBottom) {
		ev3_motor_rotate(EV3_PORT_C, 75, 100, true);		
	} else { // At Down
		ev3_motor_rotate(EV3_PORT_C, 50, 100, true);	
	}
	ArmState = ArmStateUp;
}

void InOutManager::UpARMMotor(int power) {

	// 既に上昇済なら何もしない
	if(ArmState == ArmStateUp) return;

	if(ArmState == ArmStateTop){ 
		ev3_motor_rotate(EV3_PORT_C, -10, power, true);		
	} else if(ArmState == ArmStateBottom) {
		ev3_motor_rotate(EV3_PORT_C, 75, power, true);		
	} else { // At Down
		ev3_motor_rotate(EV3_PORT_C, 50, power, true);	
	}
	ArmState = ArmStateUp;
}

void InOutManager::DownARMMotor()
{
	// 既に下降済なら何もしない
	if(ArmState == ArmStateDown) return;

	// ターゲットタイプがブロックなら床に戻す
	if(HSLTargetType == BlockColor) HSLTargetType = FieldColor;

	if(ArmState == ArmStateTop){ 
		ev3_motor_rotate(EV3_PORT_C, -60, 100, true);		
	} else if(ArmState == ArmStateBottom) {
		ev3_motor_rotate(EV3_PORT_C, 25, 100, true);		
	} else { // At UP
		ev3_motor_rotate(EV3_PORT_C, -50, 100, true);		
	}

	ArmState = ArmStateDown;
}

void InOutManager::DownARMMotor(int power) {
	// 既に下降済なら何もしない
	if(ArmState == ArmStateDown) return;
	
		// ターゲットタイプがブロックなら床に戻す
		if(HSLTargetType == BlockColor) HSLTargetType = FieldColor;
	
		if(ArmState == ArmStateTop){ 
			ev3_motor_rotate(EV3_PORT_C, -60, power, true);		
		} else if(ArmState == ArmStateBottom) {
			ev3_motor_rotate(EV3_PORT_C, 25, power, true);		
		} else { // At UP
			ev3_motor_rotate(EV3_PORT_C, -50, power, true);		
		}
	
		ArmState = ArmStateDown;
	
}

// 懸賞回収箇所までモータを下げる
void InOutManager::ARMMotorAtBottom()
{
	// 既に懸賞回収箇所なら何もしない
	if(ArmState == ArmStateBottom) return;

	if(ArmState == ArmStateTop) {
		ev3_motor_rotate(EV3_PORT_C, -85, 100, true);		
	} else if(ArmState == ArmStateDown) {
		ev3_motor_rotate(EV3_PORT_C, -25, 100, true);
	} else { // At UP
		ev3_motor_rotate(EV3_PORT_C, -75, 100, true);		

	}
	ArmState = ArmStateBottom;		
}


void InOutManager::UpTailMotor()
{
	
	if(TailState == TailStateUp) return;

	if(TailState == TailStateDown){
		ev3_motor_rotate(EV3_PORT_D, -100, 100, true);	
	} else {
		// Bottom
		ev3_motor_rotate(EV3_PORT_D, -150, 100, true);	
	}

	TailState = TailStateUp;
}

void InOutManager::DownTailMotor()
{
	// 既に下降済なら何もしない
	if(TailState == TailStateDown) return;
	
	if(TailState == TailStateUp) {
		ev3_motor_rotate(EV3_PORT_D, 100, 100, true);		
	} else {
		// BOTTOM
		ev3_motor_rotate(EV3_PORT_D, -50, 100, true);		
	}
	TailState = TailStateDown;
}

void InOutManager::BottomTailMotor() 
{
	// 既に下降済なら何もしない
	if(TailState == TailStateBottom) return;
	
	if(TailState == TailStateUp) {
		ev3_motor_rotate(EV3_PORT_D, 150, 100, true);		
	} else {
		ev3_motor_rotate(EV3_PORT_D, 50, 100, true);		
	}

	TailState = TailStateBottom;
}
	

void InOutManager::ReadInputSensor()
{
	//入力電文構造体にAPIの取得値を代入
	InputData.LeftMotorAngle = LeftMotor->getCount();
	LeftMotor->setCount(0);
	InputData.RightMotorAngle = RightMotor->getCount();
	RightMotor->setCount(0);
	
	InputData.ArmMotorAngle = ArmMotor->getCount();
	InputData.TailMotorAngle = TailMotor->getCount();
	
	InputData.TouchSensor = Touch->isPressed();
	InputData.SonarDistance = Sonar->getDistance();
	rgb_raw_t rgb;
	Color->getRawColor(rgb);
	rgb.r = uint16_t(rgb.r * (255.0  / 194.0)); 
	rgb.g = uint16_t(rgb.g * (255.0  / 318.0)); 
	rgb.b = uint16_t(rgb.b * (255.0  / 104.0));  

	// 補正後でも範囲外の場合には、桁溢れを抑止
	if(rgb.r > 255) rgb.r = 255;
	if(rgb.g > 255) rgb.g = 255;
	if(rgb.b > 255) rgb.b = 255;

	InputData.ColorRed = rgb.r;
	InputData.ColorGreen = rgb.g;
	InputData.ColorBlue = rgb.b;
	
	// 反射光を輝度値から変換
	int light = (0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b);
	if(light > 255) light = 255;

	// 反射光の値に応じて、本体のLEDライトを変更
	// 黒線より ： 赤色 
	// 白より ： オレンジ
	// いい感じ：緑
	if (light < 105) 		ev3_led_set_color(LED_RED);
	else if (light > 135)  	ev3_led_set_color(LED_ORANGE);
	else					ev3_led_set_color(LED_GREEN);

	InputData.ReflectLight = light;  
	InputData.AnglarSpeed = Gyro->getAnglerVelocity();
	InputData.SleepTime = SleepTime;
	InputData.reserved2 = 0;
	InputData.BatteryCurrent = ev3_battery_current_mA();
	InputData.BatteryVoltage = ev3_battery_voltage_mV();

	//入力信号電文構造体と同サイズのバッファに電文をコピー
	memcpy(buff_input_signal, &InputData, sizeof(InputSignalData));

	// HSL色空間に変換して、予測色を更新
	HSLColor::FromRGB(rgb.r, rgb.g, rgb.b, &HSLValue);

	// タイプを指定して更新
	HSLKind = ColorDecision::Decision(&HSLValue, HSLTargetType);
	
	HSLData.Hue = HSLValue.Hue;
	HSLData.Saturation = HSLValue.Saturation;
	HSLData.Luminosity = HSLValue.Luminosity;
	HSLData.HSLKind = HSLKind;

	// HSL情報電文構造体と同サイズのバッファに電文をコピー
	memcpy(buff_hsl_color, &HSLData, sizeof(HSLColorData));
}

void InOutManager::ResetMotor()
{
	delete RightMotor;
	delete LeftMotor;

	RightMotor = new Motor(PORT_A, false, LARGE_MOTOR);
	LeftMotor = new Motor(PORT_B, false, LARGE_MOTOR);	
}

void InOutManager::WriteOutputMotor()
{
	ev3_motor_set_speed(EV3_PORT_A, OutputData.RightMotorPower);
	ev3_motor_set_speed(EV3_PORT_B, OutputData.LeftMotorPower);

	//出力信号電文構造体と同サイズの出力電文バッファに出力電文をコピー
	memcpy(buff_output_signal, &OutputData, sizeof(OutputSignalData));
}

void InOutManager::WriteRawOutputMotor()
{
	LeftMotor->setPWM(OutputData.LeftMotorPower);
	RightMotor->setPWM(OutputData.RightMotorPower);

	//出力信号電文構造体と同サイズの出力電文バッファに出力電文をコピー
	memcpy(buff_output_signal, &OutputData, sizeof(OutputSignalData));
}


