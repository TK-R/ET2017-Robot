#include "ev3api.h"
#include "app.h"
#include <string.h>
#include <syssvc/serial.h>
#include <ColorSensor.h>
#include <Port.h>
#include <Motor.h>
#include <TouchSensor.h>
#include <SonarSensor.h>
#include <GyroSensor.h>

#include "SerialData.h"
#include "SerialReceiveTask.h"
#include "SerialReceiveState.h"

#define RECEIVE_BUFF_SIZE 1024

// 出力信号電文のデータ領域
OutputSignalData OutputData;

void serial_receive_task(intptr_t exinf)
{
	// 受信に用いるバッファ領域
	char ReceiveBuff[RECEIVE_BUFF_SIZE];
	SerialManager* Context = new SerialManager();

	while(1)
	{
		dly_tsk(10);
		int recvSize;

		// 接続中でなければ、何もしない
		if (!ev3_bluetooth_is_connected())
			continue;

		// バッファにBluetoothからのデータを格納
		recvSize = serial_rea_dat(SIO_PORT_BT, ReceiveBuff, 1);
		if(recvSize < 0)
			continue;

		for(int i = 0; i < recvSize; i++)
		{	
			Context->CurrentState->Receive((uint8_t)ReceiveBuff[i]);
		}
	}	
}



