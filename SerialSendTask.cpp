
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

void serial_send_task(intptr_t exinf)
{
	while(true)
	{
		//ヘッダの作成
		Header header;
		header.Head = HEADER_HEAD_VALUE;
		header.Size = INPUT_SIGNAL_DATA_BYTE_SIZE;
		header.Command = COMMAND_INPUT_SIGNAL_DATA;
		
		//出力電文バッファのチェックサムを計算
		uint8_t checksum = 0;
		for(uint8_t elm : buff_input_signal){
			checksum += elm;
		}

		//ヘッダ・出力電文バッファ・チェックサムを送信バッファにコピー
		char buff_send[sizeof(Header)+sizeof(InputSignalData)+sizeof(uint8_t)];
		
		memcpy(buff_send, &header, sizeof(Header));
		memcpy(buff_send+sizeof(Header), &buff_input_signal, sizeof(InputSignalData));
		memcpy(buff_send+sizeof(Header)+sizeof(InputSignalData), &checksum, sizeof(checksum));
		
		// 接続中なら、送信バッファを送信
		if (ev3_bluetooth_is_connected())
			serial_wri_dat(SIO_PORT_BT, buff_send, sizeof(buff_send));
	
		dly_tsk(20);

	}
}