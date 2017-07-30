#include <string.h>

#include "ev3api.h"
#include "SerialReceiveState.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"

// シリアル受信メソッド
void HeaderState::Receive(uint8_t data)
{
	// ヘッダコードを受信したら、ヘッダデータ領域受信ステートに遷移
	if(data == 0xff) 
	{
		Context->SetState(new HeaderDataState(Context));
	}
}

void HeaderDataState::Receive(uint8_t data)
{
	buff.push_back(data);
	if (buff.size() < sizeof(Header)) return;

	// ヘッダデータ受信完了のため、構造体を構築
	Header header;
	memcpy(&header, buff.data(), sizeof(Header));
	
	switch (header.Command)
	{
		case COMMAND_INPUT_SIGNAL_DATA:
			Context->SetState(new InputDataState(Context));
			break;
		case COMMAND_OUTPUT_SIGNAL_DATA:
			Context->SetState(new OutputDataState(Context));
			break;
		case COMMAND_PID_DATA:
			Context->SetState(new PIDDataState(Context));
			break;
		case COMMAND_BLOCK_MOVE_RULE_DATA:
			Context->SetState(new BlockMoveRuleState(Context, header.Size));
			break;
		default:
			Context->SetState(new HeaderState(Context));
			break;
	}
}


void InputDataState::Receive(uint8_t data)
{
	syslog(0, "InputData Received!");
}

void OutputDataState::Receive(uint8_t data)
{
	buff.push_back(data);
	if (buff.size() < sizeof(OutputSignalData) + 1) return;

	// ヘッダデータ受信完了のため、チェックサムを取得
	uint8_t checkSum = buff.back();
	// 末尾を削除
	buff.pop_back();
	
	// チェックサムを計算
	uint8_t calcCheckSum = 0;
	for(uint8_t elm : buff)
	{
		calcCheckSum += elm; 
	}


	if (calcCheckSum == checkSum)
	{
		// チェックサムが一致したら、構造体を構築
		OutputSignalData data;
		memcpy(&data, buff.data(), sizeof(OutputSignalData));
		TargetOutputData = data;

		InOutManager::GetInstance()->OutputData = data;
	}

	Context->SetState(new HeaderState(Context));
}

void PIDDataState::Receive(uint8_t data)
{
	buff.push_back(data);
	if (buff.size() < sizeof(PIDData) + 1) return;

	// ヘッダデータ受信完了のため、チェックサムを取得
	uint8_t checkSum = buff.back();
	// 末尾を削除
	buff.pop_back();
	
	// チェックサムを計算
	uint8_t calcCheckSum = 0;
	for(uint8_t elm : buff)
	{
		calcCheckSum += elm; 
	}


	if (calcCheckSum == checkSum)
	{
		// チェックサムが一致したら、構造体を構築
		PIDData data;
		memcpy(&data, buff.data(), sizeof(PIDData));
		CurrentPID = data;
	}

	Context->SetState(new HeaderState(Context));
}

void BlockMoveRuleState::Receive(uint8_t data)
{

}
