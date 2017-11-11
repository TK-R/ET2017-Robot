#include <math.h>

#include "SerialData.h"
#include "PIDDataManager.h"
#include "SelfPositionManager.h"
#include "StrategyManager.h"
#include "BlockMoveManager.h"
#include "BlockMoveStrategy.h"
#include "InOutManager.h"
#include "PIDDataManager.h"
#include "SoundPlayTask.h"

#define FIELD 	220
#define EDGE_LINE 120 // 黒線との境界線
#define TURN_POWER 20 // 旋回時のパワー
#define ONLINE 25	  // 黒線上での輝度値
#define NotONLINE 60  // 黒線以外での輝度値

#define APPROACH_OFFSET 60
#define MOVE_OFFSET 60

// 次のステートに切り替える
void BlockMoveStrategy::ChangeState(AbstractMoveState* nextState)
{
	delete State;
	State = nextState;
}

void BlockMoveStrategy::Run()
{
	State->Run();
}


// ブロックまで移動中の動作
void ApproachState::Run()
{
	BlockMoveManager* BtManager = BlockMoveManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
	InOutManager* IoManager = InOutManager::GetInstance();			

	PIDData BlockMovePID = PIDDataManager::GetInstance()->GetPIDData(BlockMovePIDState);
	PIDData BlockMoveHighPID = PIDDataManager::GetInstance()->GetPIDData(BlockMoveHighPIDState);
	
	int currentAngle = SpManager->RobotAngle;

	int targetWaypointAngle = BtManager->GetSrcWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
	int targetBlockAngle = BtManager->GetSrcBlockAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);

	int diffAngle = abs(targetBlockAngle - currentAngle);

	if(diffAngle > 180) {
		diffAngle = 360 - diffAngle;	
	}


	switch(SubState){
	// 初回処理
	case Initialize:
		if(BtManager->CurrentCommand.SourceBlockPosition == 9) {
			// 最寄の場合には、即ライントレース
			LeftEdge = false;
			SubState = LineTrace;	
		} else if(CurrentWayPointNo == 26) {
			// 初回であり、9ではないとき
			SubState = FirstTurn;
		} else if(CurrentWayPointNo == BtManager->GetSrcWayPointNo()) {
			// すでにウェイポイントの上にいるとき
			SubState = LineTurn;
		} else {
			SubState = FirstTurn;
		}
		break;

	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 角度が一致したため、仮想ウェイポイント間の移動に遷移	
		if (abs(targetWaypointAngle - currentAngle) < 2) {
			SubState = ImaginaryWaypoint;
			SpManager->Distance = 0;
		}

		// 旋回動作を実行
		IoManager->Turn(currentAngle, targetWaypointAngle, TURN_POWER);
		
		break;
	// ウェイポイントに向かって移動する動作
	case ImaginaryWaypoint:
		// ラインを認識した場合には、直進ステートに遷移
		if(IoManager->InputData.ReflectLight < ONLINE && SpManager->Distance > 105) {
			// ラインをまたぐまで直進
			SubState = OverLine;
		} else {
			IoManager->Forward(BlockMoveHighPID.BasePower);
		}
		break;
	// ラインをまたぐまでの処理
	case OverLine:
		if(IoManager->InputData.ReflectLight > FIELD) {	
			// ウェイポイントに到達したので、現在いるウェイポイントNoを更新
			CurrentWayPointNo = BtManager->GetSrcWayPointNo();
			
			// 白認識（ラインを跨いだ）ので、次の状態に繊維
			bool last = BtManager->ArrivalSrcWayPoint();

			// 最終ウェイポイントの場合
			if(last) {
				// ライントレース前の旋回動作に遷移
				SubState = LineTurn;
				CW = IoManager->JudgeTurnCW(currentAngle, targetBlockAngle);

			} else {
				// 初回旋回に遷移
				SubState = FirstTurn;
			}
		} else {	
			// ラインをまたぐまでは直進
			IoManager->Forward(BlockMovePID.BasePower);
		}
		break;
	// ライントレース前の旋回動作
	case LineTurn:
		// 旋回動作を実行
		if(diffAngle > 60 || IoManager->InputData.ReflectLight > EDGE_LINE){
			IoManager->Turn(CW, TURN_POWER);
		} else {
			// 角度が一致したため、ライントレースに遷移
			SubState = LineTrace;

			// 旋回方向によって、ラインのどちら側を走行するか決定	
			LeftEdge = CW;
		}
		break;	
	// ライントレースする動作	
	case LineTrace:
		if(IoManager->HSLKind == BtManager->GetSrcBlockPositionColor()) ColorDetectCount++;

		if(ColorDetectCount < 10) {				
			// ライントレース中は、ラインの角度に修正
			int angle = BtManager->GetLine(BtManager->GetSrcWayPointNo())->GetAngle(BtManager->CurrentCommand.SourceBlockPosition);
			SpManager->ResetAngle(angle);

			// 初回のみ高速移動
			if(BtManager->CurrentCommand.SourceBlockPosition == 9) {
				IoManager->LineTraceAction(BlockMoveHighPID, EDGE_LINE, LeftEdge);		
			} else {
				IoManager->LineTraceAction(BlockMovePID, EDGE_LINE, LeftEdge);		
			}
			break;
		}

		if(ColorDetectCount == 10) {
			IoManager->Stop();
			IoManager->WriteOutputMotor();
			dly_tsk(150);

			// ブロック置き場の座標に修正
			Point *p = BtManager->GetSrcBlockPoint();
			SpManager->ResetX(p->X - cos(SpManager->RobotAngle * M_PI / 180.0) * APPROACH_OFFSET);
			SpManager->ResetY(p->Y + sin(SpManager->RobotAngle * M_PI / 180.0) * APPROACH_OFFSET);

			// ブロック運搬時の経路がある（最終コマンドではない）場合には、ブロック運搬ステートに遷移
			if(BtManager->CurrentCommand.BlockMoveWaypointCount != 0) {
				// ブロック置き場到達メッセージ
				BtManager->ArrivalSrcBlockPosition();

				auto moveState = new MoveState(ParentStrategy);
				moveState->LeftEdge = LeftEdge;
				int nextWaypointAngle = BtManager->GetDstWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
				
				// ブロック運搬は初回直進処理
				moveState->SubState = FirstStraight;

				if(CurrentWayPointNo == 26) {
					moveState->CW = IoManager->JudgeTurnCW(currentAngle, nextWaypointAngle);		
				} else {
					moveState->CW = IoManager->JudgeTurnCW(currentAngle, nextWaypointAngle);		
				}

				moveState->CurrentWayPointNo = CurrentWayPointNo;
				ParentStrategy->ChangeState(moveState);	

				// 積算距離をリセット
				SpManager->Distance = 0;
			} else {
				IoManager->Stop();
			}
			break;
		}
		break;
	
	case FirstStraight:
	case FirstLineTrace:	
	case Back:
		break;
	}
}

void MoveState::Run()
{
	BlockMoveManager* BtManager = BlockMoveManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
	InOutManager* IoManager = InOutManager::GetInstance();			
	SpManager->ParticleFilterON = false;

	PIDData BlockMovePID = PIDDataManager::GetInstance()->GetPIDData(BlockMovePIDState);
	PIDData BlockMoveHighPID = PIDDataManager::GetInstance()->GetPIDData(BlockMoveHighPIDState);
	
	int currentAngle = SpManager->RobotAngle;
	int dstWayPointNo = BtManager->GetDstWayPointNo();
	int moveDistance = 0;
	int targetWaypointAngle = BtManager->GetDstWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
	int targetBlockAngle = BtManager->GetDstBlockAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);

	int diffAngle = abs(BtManager->GetLine(CurrentWayPointNo)->GetAngle(BtManager->CurrentCommand.SourceBlockPosition)
			 			- BtManager->GetLine(dstWayPointNo)->GetAngleWithSource(BtManager->CurrentCommand.SourceBlockPosition));

	if(diffAngle > 180) {
		diffAngle = 360 - diffAngle;	
	}

	
	switch(SubState){
	// 初回直進処理 
	case FirstStraight:
		// 次の移動先ウェイポイントとの為す角度によって、前進距離を切り替える
		if(diffAngle < 5){
			moveDistance = 80;
		} else if(diffAngle < 75) {
			moveDistance = 45;
		} else if (diffAngle < 110) {
				moveDistance = 45;
		} else {
			moveDistance = 45;
		}
	
		if(SpManager->Distance > moveDistance) {
			// 初回処理に遷移する
			SubState = Initialize;
		}
		IoManager->Forward(BlockMovePID.BasePower);
		break;

	// 初回処理
	case Initialize:
		// ラインの角度を目標値として設定
		targetWaypointAngle = BtManager->GetLine(dstWayPointNo)->GetAngleWithSource(BtManager->CurrentCommand.SourceBlockPosition);
	
		// 角度が一致したため、仮想ウェイポイント間の移動に遷移	
		if (abs(targetWaypointAngle - currentAngle) < 3) {
			SubState = FirstLineTrace;
			SpManager->Distance = 0;			
			break;
		}
		if(diffAngle < 75) {
			LeftEdge = !CW;
		} else {
			LeftEdge = CW;
		}
		// 旋回動作を実行
		IoManager->TurnWithBlock(CW, TURN_POWER ,  -0.3);
		break;

	case FirstLineTrace:
	//	ラインに沿って進む
		{
		int moveDistance = 180;
		
		// 長いラインの場合には存分に移動する
		if(BtManager->GetDstWayPointNo() == 9 || BtManager->GetDstWayPointNo() == 14) {
			moveDistance = 270;
		} else if(BtManager->GetDstWayPointNo() == 24) {
			moveDistance = 150;
		}

		if(SpManager->Distance > moveDistance) {
			// ウェイポイントに到達したので、現在いるウェイポイントNoを更新
			CurrentWayPointNo = BtManager->GetDstWayPointNo();
			
			// 次の状態に繊維
			bool last = BtManager->ArrivalDstWayPoint();

			// 最終ウェイポイントの場合
			if(last) {
				// ライントレースに遷移
				SubState = LineTrace;
			} else {
				// 初回旋回に遷移
				SubState = FirstTurn;
			}
			break;
		}
		}
		// ライントレース中は、ラインの角度に修正
		{
			int angle = BtManager->GetLine(dstWayPointNo)->GetAngleWithSource(BtManager->CurrentCommand.SourceBlockPosition);
			SpManager->ResetAngle(angle);
		}			
		
		IoManager->LineTraceAction(BlockMovePID, EDGE_LINE, LeftEdge);
		break;
	
	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 角度が一致したため、仮想ウェイポイント間の移動に遷移	
		if (abs(targetWaypointAngle - currentAngle) < 2) {
			SubState = ImaginaryWaypoint;				

			// 移動距離をクリア
			SpManager->Distance = 0;
			break;
		}
		CW = IoManager->JudgeTurnCW(currentAngle, targetWaypointAngle);

		// 旋回動作を実行
		IoManager->TurnWithBlock(CW, TURN_POWER, -0.3);
		break;
	// ウェイポイントに向かって移動する動作
	case ImaginaryWaypoint:
		// 一定距離進んだ後、ラインを認識した場合には、直進ステートに遷移
		if(IoManager->InputData.ReflectLight < ONLINE && SpManager->Distance > 105) {
			// ラインをまたぐまで直進
			SubState = OverLine;
		} else {
			IoManager->Forward(BlockMoveHighPID.BasePower);
		}

		//目標とするブロック置き場の色を取得したら、現在のステートをラインとレース中に変更
		if(IoManager->HSLKind == BtManager->GetDstBlockPositionColor()) ColorDetectCount++;
		if(IoManager->HSLKind == HSLBlack) ColorDetectCount = 0;
		
		if(ColorDetectCount > 8) {
			SubState = LineTrace;
		}
		
		break;
	// ラインをまたぐまでの処理
	case OverLine:
		if(IoManager->InputData.ReflectLight > EDGE_LINE) {	
			// ウェイポイントに到達したので、現在いるウェイポイントNoを更新
			CurrentWayPointNo = BtManager->GetDstWayPointNo();
			
			// 白認識（ラインを跨いだ）ので、次の状態に繊維
			bool last = BtManager->ArrivalDstWayPoint();
			// 最終ウェイポイントの場合
			if(last) {
				// ライントレース前の旋回動作に遷移
				SubState = LineTurn;

				// 旋回方向を判定
				CW = IoManager->JudgeTurnCW(currentAngle, targetBlockAngle);
			} else {
				// 初回旋回に遷移
				SubState = FirstTurn;
			}
		} else {	
			// ラインをまたぐまでは直進
			IoManager->Forward(BlockMovePID.BasePower);
		}
		break;
	// ライントレース前の旋回動作
	case LineTurn:
		{
			int diff = abs(targetBlockAngle - currentAngle);
			if(diff > 180)
				diff = 360 - diff; 

			if(diff < 70 && (IoManager->InputData.ReflectLight < EDGE_LINE || IoManager->HSLKind == BtManager->GetDstBlockPositionColor()))
			{
				// 角度が一致したため、ライントレースに遷移
				SubState = LineTrace;
				
				// 旋回方向によって、ラインのどちら側を走行するか決定	
				LeftEdge = CW;
				break;
			}
		}
			
		// 旋回動作を実行	
		IoManager->TurnWithBlock(CW, TURN_POWER, -0.3);
		break;	
	// ライントレースする動作	
	case LineTrace:
		//目標とするブロック置き場の色を取得したら、現在のステートをブロック運搬中に変更
		if(IoManager->HSLKind == BtManager->GetDstBlockPositionColor()) ColorDetectCount++;
		if(IoManager->HSLKind == HSLBlack) ColorDetectCount = 0;

		if(ColorDetectCount > 8)
		{
			IoManager->Stop();
			IoManager->WriteOutputMotor();
			dly_tsk(150);

			// ブロック置き場の座標に修正			
			Point *p = BtManager->GetDstBlockPoint();
			SpManager->ResetX(p->X - cos(SpManager->RobotAngle * M_PI / 180.0) * MOVE_OFFSET);
			SpManager->ResetY(p->Y + sin(SpManager->RobotAngle * M_PI / 180.0) * MOVE_OFFSET);

			SpManager->Distance = 400;
			SubState = Back;
			break;
		}
		
		// ライントレース中は、ラインの角度に修正
		{
			int angle = BtManager->GetLine(CurrentWayPointNo)->GetAngle(BtManager->CurrentCommand.DestinationBlockPosition);
			SpManager->ResetAngle(angle);
		}			
		IoManager->LineTraceAction(BlockMovePID, EDGE_LINE, LeftEdge);
		
		break;
	// 後退する動作
	case Back:
		if(SpManager->Distance < 320) {
			// 17cm後退したら、アプローチに遷移する

			// ブロック置き場到達メッセージ
			bool last = BtManager->ArrivalDstBlockPosition();

			// 最後までやったら、やりきった感を出す
			if (last) {
				IoManager->Stop();
				IoManager->WriteOutputMotor();
				PlaySound(BlockEnd);
				
				ParentStrategy->Manager->SetStrategy(NULL);
				break;
			} 
			
			auto approachState = new ApproachState(ParentStrategy);
			approachState->CurrentWayPointNo = CurrentWayPointNo;
			ParentStrategy->ChangeState(approachState);
			approachState->CW = CW;

			break;
		}
		IoManager->Back(BlockMovePID.BasePower);
		break;
	}

}

