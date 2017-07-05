#ifndef __SERIAL_RECV_TASK_H__
#define __SERIAL_RECV_TASK_H__

#include "SerialData.h"
#include "SerialReceiveState.h"

// 出力信号電文のデータ領域。本体はSerialReceive.cppにて定義
extern OutputSignalData OutputData;

class SerialState;

class SerialManager
{
public:
	// 現在のシリアル受信ステート
	SerialState *CurrentState;

	SerialManager(){
		CurrentState = new HeaderState(this);
	}
};

#endif