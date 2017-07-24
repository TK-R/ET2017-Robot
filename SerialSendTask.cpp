
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
uint8_t buff_input_signal[sizeof(InputSignalData)];

// 出力信号電文のデータ領域
uint8_t buff_output_signal[sizeof(OutputSignalData)];

// 自己位置情報電文のデータ領域
uint8_t buff_self_position[sizeof(SelfPositionData)];

// 電文送信処理
void send_data(uint8_t buff[], uint8_t Command, int dataSize)
{
	//ヘッダの作成
	Header header;
	header.Head = HEADER_HEAD_VALUE;
	header.Size = dataSize;
	header.Command = Command;
	
	//入力電文バッファのチェックサムを計算
	uint8_t checksum = 0;

	for(int i = 0; i < dataSize; i++){
		checksum += buff[i];
	}

	//ヘッダ・出力電文バッファ・チェックサムを送信バッファにコピー
	char buff_send[sizeof(Header)+dataSize+sizeof(uint8_t)];
	
	memcpy(buff_send, &header, sizeof(Header));
	memcpy(buff_send+sizeof(Header), buff, dataSize);
	memcpy(buff_send+sizeof(Header)+dataSize, &checksum, sizeof(checksum));
	
	// 接続中なら、送信バッファを送信
	if (ev3_bluetooth_is_connected())
		serial_wri_dat(SIO_PORT_BT, buff_send, sizeof(buff_send));
}


void serial_send_task(intptr_t exinf)
{
	while(true)
	{
		send_data(buff_input_signal, COMMAND_INPUT_SIGNAL_DATA, sizeof(InputSignalData));
		send_data(buff_output_signal, COMMAND_OUTPUT_SIGNAL_DATA, sizeof(OutputSignalData));
		send_data(buff_self_position, COMMAND_SELF_POSITION_DATA, sizeof(SelfPositionData));

		dly_tsk(20);

	}
}