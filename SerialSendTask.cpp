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

		Header header;
		header.Head = HEADER_HEAD_VALUE;
		header.Size = OUTPUT_SIGNAL_DATA_BYTE_SIZE;
		header.Command = COMMAND_OUTPUT_SIGNAL_DATA;

		OutputSignalData out_data;
		/*
		out_data.FrontMotorPower = front_motor->getPWM();
		out_data.RightMotorPower = right_motor->getPWM();
		out_data.LeftMotorPower = left_motor->getPWM();
		out_data.BackMotorPower = back_motor->getPWM();
*/
		char buff[sizeof(Header)+sizeof(OutputSignalData)+sizeof(uint8_t)];
		memcpy(buff, &header, sizeof(Header));
		memcpy(buff+sizeof(Header), &out_data, sizeof(OutputSignalData));
		buff[7] = 6;

		//ヘッダをシリアライズ
		//データをシリアライズ
		//データのチェックサムを計算
		//ヘッダとデータとチェックサムを結合
		//送信

		serial_wri_dat(SIO_PORT_BT, buff, 8);
//		fprintf(bt, "\r\nEcho test is started.\r\n");

		tslp_tsk(100);
	}
}