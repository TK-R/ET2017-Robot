#include "ev3api.h"
#include "app.h"
#include "SerialSendTask.h"
#include <string.h>
#include <syssvc/serial.h>
#include <ColorSensor.h>
#include <Port.h>
#include <Motor.h>
#include <TouchSensor.h>
#include <SonarSensor.h>
#include <GyroSensor.h>
//#include <ev3api_sensor.h>

using ev3api::Motor;
using ev3api::TouchSensor;
using ev3api::SonarSensor;
using ev3api::GyroSensor;
using ev3api::ColorSensor;

void serial_send_task(intptr_t exinf)
{

//	FILE *bt = ev3_serial_open_file(EV3_SERIAL_BT);
//   fprintf(bt, "\r\nEcho test is started.\r\n");

	//ポート割り当て
	static const ePortM front_motor_port = PORT_A;
	static const ePortM right_motor_port = PORT_B;
	static const ePortM left_motor_port = PORT_C;
	static const ePortM back_motor_port = PORT_D;
	static const bool front_motor_brake_mode = false;
	static const bool right_motor_brake_mode = false;
	static const bool left_motor_brake_mode = false;
	static const bool back_motor_brake_mode = false;
	static const motor_type_t front_motor_type = LARGE_MOTOR;
	static const motor_type_t right_motor_type = LARGE_MOTOR;
	static const motor_type_t left_motor_type = LARGE_MOTOR;
	static const motor_type_t back_motor_type = MEDIUM_MOTOR;
	static const ePortS touch_sensor_port = PORT_1;
	static const ePortS sonar_sensor_port = PORT_2;
	static const ePortS color_sensor_port = PORT_3;
	static const ePortS gyro_sensor_port = PORT_4;

	//インスタンス生成
	Motor* front_motor = new Motor(front_motor_port, front_motor_brake_mode, front_motor_type);
	Motor* right_motor = new Motor(right_motor_port, right_motor_brake_mode, right_motor_type);
	Motor* left_motor = new Motor(left_motor_port, left_motor_brake_mode, left_motor_type);
	Motor* back_motor = new Motor(back_motor_port, back_motor_brake_mode, back_motor_type);
	TouchSensor* touch_sensor = new TouchSensor(touch_sensor_port);
	SonarSensor* sonar_sensor = new SonarSensor(sonar_sensor_port);
	ColorSensor* color_sensor = new ColorSensor(color_sensor_port);
	GyroSensor *gyro_sensor = new GyroSensor(gyro_sensor_port);

	while(true)
	{
		if (!ev3_bluetooth_is_connected())
			continue;

		//ヘッダの作成
		Header header;
		header.Head = HEADER_HEAD_VALUE;
		header.Size = INPUT_SIGNAL_DATA_BYTE_SIZE;
		header.Command = COMMAND_INPUT_SIGNAL_DATA;
		//入力電文構造体にAPIの取得値を代入
		InputSignalData input_signal_data;
		input_signal_data.FrontMotorAngle = front_motor->getCount();
		front_motor->setCount(0);
		input_signal_data.RightMotorAngle = right_motor->getCount();
		right_motor->setCount(0);
		input_signal_data.LeftMotorAngle = left_motor->getCount();
		left_motor->setCount(0);
		input_signal_data.BackMotorAngle = back_motor->getCount();
		back_motor->setCount(0);
		
		input_signal_data.TouchSensor = touch_sensor->isPressed();
		input_signal_data.SonarDistance = sonar_sensor->getDistance();
		rgb_raw_t rgb;
		color_sensor->getRawColor(rgb);
		input_signal_data.ColorRed = rgb.r;
		input_signal_data.ColorGreen = rgb.g;
		input_signal_data.ColorBlue = rgb.b;
		input_signal_data.AmbientLight = color_sensor->getAmbient();
		input_signal_data.ReflectLight = color_sensor->getBrightness();
		input_signal_data.AccelX = gyro_sensor->getAnglerVelocity();
		input_signal_data.AccelY = gyro_sensor->getAngle();
		input_signal_data.AccelZ = 0;
		input_signal_data.Temperature = 0;
		input_signal_data.BatteryCurrent = ev3_battery_current_mA();
		input_signal_data.BatteryVoltage = ev3_battery_voltage_mV();
		//出力電文構造体と同サイズの出力電文バッファに出力電文をコピー
		uint8_t buff_input_signal[sizeof(InputSignalData)];
		memcpy(buff_input_signal, &input_signal_data, sizeof(InputSignalData));
		//出力電文バッファのチェックサムを計算
		uint8_t checksum = 0;
		for(uint8_t elm : buff_input_signal){
			checksum += elm;
		}
		//ヘッダ・出力電文バッファ・チェックサムを送信バッファにコピー
		char buff_send[sizeof(Header)+sizeof(InputSignalData)+sizeof(uint8_t)];
		memcpy(buff_send, &header, sizeof(Header));
		memcpy(buff_send+sizeof(Header), &input_signal_data, sizeof(InputSignalData));
		memcpy(buff_send+sizeof(Header)+sizeof(InputSignalData), &checksum, sizeof(checksum));
		//送信バッファを送信
		serial_wri_dat(SIO_PORT_BT, buff_send, sizeof(buff_send));

//		fprintf(bt, "\r\nEcho test is started.\r\n");

		tslp_tsk(100);
	}
}