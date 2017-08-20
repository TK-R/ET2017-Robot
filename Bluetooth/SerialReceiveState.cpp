#include <string.h>

#include "ev3api.h"
#include "SerialReceiveState.h"
#include "SerialReceiveTask.h"
#include "InOutManager.h"
#include "SelfPositionManager.h"
#include "BlockMoveManager.h"
#include "PIDDataManager.h"

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
		case COMMAND_SELF_POSITION_DATA:
			Context->SetState(new PositionDataState(Context));
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

// 出力信号電文ステートにおける受信処理
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

// PID電文受信ステートにおける受信処理
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

		auto pidManager = PIDDataManager::GetInstance();
		pidManager->SetPIDData(data);

		//		CurrentPID = data;
	}

	Context->SetState(new HeaderState(Context));
}

// 自己位置情報電文受信ステートにおける受信処理
void PositionDataState::Receive(uint8_t data)
{
	buff.push_back(data);
	if (buff.size() < sizeof(SelfPositionData) + 1) return;

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
		SelfPositionData pData;
		memcpy(&pData, buff.data(), sizeof(SelfPositionData));
		SelfPositionManager::GetInstance()->ResetPosition(pData);
	}

	Context->SetState(new HeaderState(Context));
}

// 運搬ルール受信ステートにおける受信処理
void BlockMoveRuleState::Receive(uint8_t data)
{
	buff.push_back(data);
	if (buff.size() < Size + 1) return;

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
		BlockMoveRuleData rule;
		
		rule.CommandSize = buff[0];
		int offset = 1;
		for(int i = 0; i < rule.CommandSize; i++) {
			BlockMoveCommandData command;
			
			// 運搬元/運搬先のブロック置き場を登録
			command.BlockColor = buff[offset++];
			command.SourceBlockPosition = buff[offset++];
			command.DestinationBlockPosition = buff[offset++];

			// 運搬元ブロック置き場へ向かう際の経路を登録
			command.ApproachWaypointCount = buff[offset++];
			for(int j = 0; j < command.ApproachWaypointCount; j++) {
				command.ApproachWayPoint[j] = buff[offset++];
			}

			// 運搬先ブロック置き場へ向かう際の経路を登録　
			command.BlockMoveWaypointCount = buff[offset++];
			for(int k = 0; k < command.BlockMoveWaypointCount; k++) {
				command.BlockMoveWayPoint[k] = buff[offset++];
			}

			// 構築したブロック運搬コマンドをブロック運搬ルールに追加
			rule.Command.push_back(command);
		}
		
		// 構築したブロック運搬ルールをブロック運搬管理クラスに登録
		BlockMoveManager::GetInstance()->SetBlockMoveRule(rule);

	}


	Context->SetState(new HeaderState(Context));

}
