#ifndef __SERIAL_RECV_TASK_H__
#define __SERIAL_RECV_TASK_H__

#include "SerialData.h"
#include "SerialReceiveState.h"
#include <cstdlib>
#include <memory>
#include <iostream>


// 出力信号電文のデータ領域。本体はSerialReceive.cppにて定義
extern OutputSignalData OutputData;

// 現在のPIDゲイン情報
extern PIDData CurrentPID;

class SerialState;

class SerialManager
{

private:
	// 現在のシリアル受信ステート
	SerialState *CurrentState;

public:
	SerialManager(){
		CurrentState = new HeaderState(this);
	}

	void Receive(uint8_t data)
	{
		CurrentState->Receive(data);
	}

	void SetState(SerialState *state){
		delete CurrentState;
		CurrentState = state;
	}
};

#endif