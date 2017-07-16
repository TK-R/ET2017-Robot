#include "ev3api.h"
#include <string.h>

#include "InOutManager.h"
#include "SerialSendTask.h"

InOutManager::InOutManager()
{
	LeftMotor = new Motor(PORT_A, false, LARGE_MOTOR);
	RightMotor = new Motor(PORT_B, false, LARGE_MOTOR);
	ArmMotor = new Motor(PORT_C, false, LARGE_MOTOR);
	TailMotor = new Motor(PORT_D, false, MEDIUM_MOTOR);

	Touch = new TouchSensor(PORT_1);
	Sonar = new SonarSensor(PORT_2);
	Color = new ColorSensor(PORT_3);
	Gyro = new GyroSensor(PORT_4);
}


void InOutManager::ReadInputSensor()
{
	//入力電文構造体にAPIの取得値を代入
	InputData.LeftMotorAngle = LeftMotor->getCount();
	LeftMotor->setCount(0);
	InputData.RightMotorAngle = RightMotor->getCount();
	RightMotor->setCount(0);
	InputData.ArmMotorAngle = ArmMotor->getCount();
	ArmMotor->setCount(0);
	InputData.TailMotorAngle = TailMotor->getCount();
	TailMotor->setCount(0);
	
	InputData.TouchSensor = Touch->isPressed();
	InputData.SonarDistance = Sonar->getDistance();
	rgb_raw_t rgb;
	Color->getRawColor(rgb);
	rgb.r = uint16_t(rgb.r * (255.0  / 194.0)); 
	rgb.g = uint16_t(rgb.g * (255.0  / 318.0)); 
	rgb.b = uint16_t(rgb.b * (255.0  / 104.0));  

	InputData.ColorRed = rgb.r;
	InputData.ColorGreen = rgb.g;
	InputData.ColorBlue = rgb.b;
	
	// 反射光を輝度値から変換
	int light = (0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b);
	if(light > 255) light = 255;

	InputData.ReflectLight = light;  

	syslog(0, "Light is %d", InputData.ReflectLight);
//	InputData.AmbientLight = color_sensor->getAmbient();
	InputData.Angle = Gyro->getAngle();
	InputData.AnglarSpeed = Gyro->getAnglerVelocity();
	InputData.reserved1 = 0;
	InputData.reserved2 = 0;
	InputData.BatteryCurrent = ev3_battery_current_mA();
	InputData.BatteryVoltage = ev3_battery_voltage_mV();


	//出力電文構造体と同サイズの出力電文バッファに出力電文をコピー
	memcpy(buff_input_signal, &InputData, sizeof(InputSignalData));}


