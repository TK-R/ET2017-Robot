
#include "ev3api.h"
#include "app.h"
#include "SerialData.h"
#include "SerialSendTask.h"
#include "HSLColor.h"

#include <string.h>
#include <syssvc/serial.h>
#include <ColorSensor.h>
#include <Port.h>
#include <Motor.h>
#include <TouchSensor.h>
#include <SonarSensor.h>
#include <GyroSensor.h>

using ev3api::Motor;
using ev3api::TouchSensor;
using ev3api::SonarSensor;
using ev3api::GyroSensor;
using ev3api::ColorSensor;

// 入力信号電文のデータ領域
InputSignalData InputData;


void serial_send_task(intptr_t exinf)
{
	//ポート割り当て
	static const ePortM left_motor_port = PORT_A;
	static const ePortM right_motor_port = PORT_B;
	static const ePortM arm_motor_port = PORT_C;
	static const ePortM tail_motor_port = PORT_D;
	static const bool left_motor_brake_mode = false;
	static const bool right_motor_brake_mode = false;
	static const bool arm_motor_brake_mode = false;
	static const bool tail_motor_brake_mode = false;
	static const motor_type_t left_motor_type = LARGE_MOTOR;
	static const motor_type_t right_motor_type = LARGE_MOTOR;
	static const motor_type_t arm_motor_type = LARGE_MOTOR;
	static const motor_type_t tail_motor_port_type = MEDIUM_MOTOR;
	static const ePortS touch_sensor_port = PORT_1;
	static const ePortS sonar_sensor_port = PORT_2;
	static const ePortS color_sensor_port = PORT_3;
	static const ePortS gyro_sensor_port = PORT_4;

	//インスタンス生成
	Motor* left_motor = new Motor(left_motor_port, left_motor_brake_mode, left_motor_type);
	Motor* right_motor = new Motor(right_motor_port, right_motor_brake_mode, right_motor_type);
	Motor* arm_motor = new Motor(arm_motor_port, arm_motor_brake_mode, arm_motor_type);
	Motor* tail_motor = new Motor(tail_motor_port, tail_motor_brake_mode, tail_motor_port_type);
	
	TouchSensor* touch_sensor = new TouchSensor(touch_sensor_port);
	SonarSensor* sonar_sensor = new SonarSensor(sonar_sensor_port);
	ColorSensor* color_sensor = new ColorSensor(color_sensor_port);
	GyroSensor *gyro_sensor = new GyroSensor(gyro_sensor_port);

	gyro_sensor->reset();

	while(true)
	{
		//ヘッダの作成
		Header header;
		header.Head = HEADER_HEAD_VALUE;
		header.Size = INPUT_SIGNAL_DATA_BYTE_SIZE;
		header.Command = COMMAND_INPUT_SIGNAL_DATA;

		//入力電文構造体にAPIの取得値を代入
		InputData.FrontMotorAngle = left_motor->getCount();
		left_motor->setCount(0);
		InputData.RightMotorAngle = right_motor->getCount();
		right_motor->setCount(0);
		InputData.LeftMotorAngle = arm_motor->getCount();
		arm_motor->setCount(0);
		InputData.BackMotorAngle = tail_motor->getCount();
		tail_motor->setCount(0);
		
		InputData.TouchSensor = touch_sensor->isPressed();
		InputData.SonarDistance = sonar_sensor->getDistance();
		rgb_raw_t rgb;
		color_sensor->getRawColor(rgb);
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
		InputData.Angle = gyro_sensor->getAngle();
		InputData.AnglarSpeed = gyro_sensor->getAnglerVelocity();
		InputData.reserved1 = 0;
		InputData.reserved2 = 0;
		InputData.BatteryCurrent = ev3_battery_current_mA();
		InputData.BatteryVoltage = ev3_battery_voltage_mV();

		//出力電文構造体と同サイズの出力電文バッファに出力電文をコピー
		uint8_t buff_input_signal[sizeof(InputSignalData)];
		memcpy(buff_input_signal, &InputData, sizeof(InputSignalData));
		
		//出力電文バッファのチェックサムを計算
		uint8_t checksum = 0;
		for(uint8_t elm : buff_input_signal){
			checksum += elm;
		}

		//ヘッダ・出力電文バッファ・チェックサムを送信バッファにコピー
		char buff_send[sizeof(Header)+sizeof(InputSignalData)+sizeof(uint8_t)];
		memcpy(buff_send, &header, sizeof(Header));
		memcpy(buff_send+sizeof(Header), &InputData, sizeof(InputSignalData));
		memcpy(buff_send+sizeof(Header)+sizeof(InputSignalData), &checksum, sizeof(checksum));
		
		
		// 接続中なら、送信バッファを送信
		if (ev3_bluetooth_is_connected())
			serial_wri_dat(SIO_PORT_BT, buff_send, sizeof(buff_send));
	
		dly_tsk(20);

	}
}