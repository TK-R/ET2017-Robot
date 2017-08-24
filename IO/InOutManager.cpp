#include "ev3api.h"
#include <string.h>

#include "SerialData.h"
#include "HSLColor.h"
#include "ColorDecision.h"
#include "InOutManager.h"
#include "SerialSendTask.h"

InOutManager::InOutManager()
{
	LeftMotor = new Motor(PORT_A, false, LARGE_MOTOR);
	RightMotor = new Motor(PORT_B, false, LARGE_MOTOR);
	ArmMotor = new Motor(PORT_C, false, LARGE_MOTOR);
	TailMotor = new Motor(PORT_D, false, MEDIUM_MOTOR);

	// アーム位置の初期化を行う
	ev3_motor_set_power(EV3_PORT_C, -15);	
	Color = new ColorSensor(PORT_3);

	ArmMotor->setCount(0);
	ev3_motor_rotate(EV3_PORT_C, 35, 30, true);
	ev3_motor_stop(EV3_PORT_C, true);
	ArmMotor->setCount(0);

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
	OutputData.LeftMotorPower = power;
	OutputData.RightMotorPower = power;
}

// 出力値と操舵角から左右モータの値を更新する（0：直進 100：右方向へ -100：左方向へ）
void InOutManager::Forward(int power, int steering)
{
	
	// ステアリング値は0-100の範囲内
	if(steering > 100) steering = 100;
	if(steering < -100) steering = -100;

	// 右側へ
	if (steering > 0)
	{
		OutputData.LeftMotorPower = (int8_t)power;
		OutputData.RightMotorPower = (int8_t)(power * (100.0 - steering) / 100.0);
	}
	// 左側へ
	else
	{
		OutputData.LeftMotorPower = (int8_t)(power * (100.0 + steering) / 100.0);
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
	OutputData.LeftMotorPower = 0;
	OutputData.RightMotorPower = 0;
}

void InOutManager::Turn(int currentAngle, int targetAngle, int power)
{
	if(abs(currentAngle-targetAngle) < 20) power = 10;

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

// ライントレースを実施するように左右モータ値を更新する
void InOutManager::LineTraceAction(PIDData data, int center, bool LeftEdge)
{
	int light = InputData.ReflectLight;

	int diff = LeftEdge ? (int)light - center :  (int)center - light;
	int steering = data.PGain * diff + (diff - PrevDiff) * data.IGain;
	
	Forward(data.BasePower, steering);
}

void InOutManager::UpARMMotor()
{
	// 既に上昇済みなら何もしない
	if(ArmUp) return;

	ev3_motor_rotate(EV3_PORT_C, 70, 30, true);	
	ArmUp = true;
}

void InOutManager::DownARMMotor()
{
	// 既に下降済なら何もしない
	if(!ArmUp) return;

	// ターゲットタイプがブロックなら床に戻す
	if(HSLTargetType == BlockColor) HSLTargetType = FieldColor;

	ev3_motor_rotate(EV3_PORT_C, -70, 30, true);		
	ArmUp = false;
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

	InputData.ReflectLight = light;  
	InputData.Angle = Gyro->getAngle();
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


void InOutManager::WriteOutputMotor()
{
	LeftMotor->setPWM(OutputData.LeftMotorPower);
	RightMotor->setPWM(OutputData.RightMotorPower);

	//出力信号電文構造体と同サイズの出力電文バッファに出力電文をコピー
	memcpy(buff_output_signal, &OutputData, sizeof(OutputSignalData));
}


