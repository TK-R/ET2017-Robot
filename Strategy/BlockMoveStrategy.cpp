
#include "SerialData.h"
#include "PIDDataManager.h"
#include "SelfPositionManager.h"
#include "BlockMoveManager.h"
#include "BlockMoveStrategy.h"
#include "InOutManager.h"
#include "PIDDataManager.h"

#define EDGE_LINE 120 // 黒線との境界線
#define TURN_POWER 15 // 旋回時のパワー
#define ONLINE 25	  // 黒線上での輝度値
#define NotONLINE 60  // 黒線以外での輝度値


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

	int currentAngle = SpManager->RobotAngle;
	int ForwardPower = 10;

	int targetWaypointAngle = BtManager->GetSrcWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
	int targetBlockAngle = BtManager->GetSrcBlockAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);

	switch(SubState){
	// 初回処理
	case Initialize:
		// すでにウェイポイント上にいる場合には、ライン上での旋回に遷移（必ず最終ウェイポイントのため）
		if(CurrentWayPointNo == BtManager->GetSrcWayPointNo()) {
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
		}

		// 旋回動作を実行
		IoManager->Turn(currentAngle, targetWaypointAngle, TURN_POWER);
		
		break;
	// ウェイポイントに向かって移動する動作
	case ImaginaryWaypoint:
		// ラインを認識した場合には、直進ステートに遷移
		if(IoManager->InputData.ReflectLight < ONLINE) {
			// ラインをまたぐまで直進
			SubState = OverLine;

			// ラインを跨いだので、ウェイポイントの座標に変更
			SpManager->ResetPoint(BtManager->GetLine(BtManager->GetSrcWayPointNo())->WayPoint);

		} else {
			IoManager->Forward(ForwardPower);
		}
		break;
	// ラインをまたぐまでの処理
	case OverLine:
		if(IoManager->InputData.ReflectLight > EDGE_LINE) {	
			// ウェイポイントに到達したので、現在いるウェイポイントNoを更新
			CurrentWayPointNo = BtManager->GetSrcWayPointNo();
			
			// 白認識（ラインを跨いだ）ので、次の状態に繊維
			bool last = BtManager->ArrivalSrcWayPoint();

			// 最終ウェイポイントの場合
			if(last) {
				// ライントレース前の旋回動作に遷移
				SubState = LineTurn;
			} else {
				// 初回旋回に遷移
				SubState = FirstTurn;
			}
		} else {	
			// ラインをまたぐまでは直進
			IoManager->Forward(ForwardPower);
		}
		break;
	// ライントレース前の旋回動作
	case LineTurn:
		// 旋回動作を実行
		if(abs(targetBlockAngle - currentAngle) > 45 || IoManager->InputData.ReflectLight > EDGE_LINE){
			IoManager->Turn(currentAngle, targetBlockAngle, TURN_POWER);
		} else {
			// 角度が一致したため、ライントレースに遷移
			SubState = LineTrace;

			// 旋回方向によって、ラインのどちら側を走行するか決定	
			if(targetBlockAngle > currentAngle) LeftEdge = false;
			else LeftEdge = true;
		}
		break;	
	// ライントレースする動作	
	case LineTrace:
		//目標とするブロック置き場の色を取得したら、現在のステートをブロック運搬中に変更
		if(IoManager->HSLKind == BtManager->GetSrcBlockPositionColor()){
			// ブロック置き場の座標に修正
			SpManager->ResetPoint(BtManager->GetSrcBlockPoint());

			auto moveState = new MoveState(ParentStrategy);
			moveState->CurrentWayPointNo = CurrentWayPointNo;
			ParentStrategy->ChangeState(moveState);
			break;
		}
		
		// ライントレース中は、ラインの角度に修正
		{
			int angle = BtManager->GetLine(BtManager->GetSrcWayPointNo())->GetAngle(BtManager->GetSrcWayPointNo());
			SpManager->ResetAngle(angle);
		}
		IoManager->LineTraceAction(BlockMovePID, EDGE_LINE, LeftEdge);
		break;
	
	case Back:
		break;
	}
}

void MoveState::Run()
{
	BlockMoveManager* BtManager = BlockMoveManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
	InOutManager* IoManager = InOutManager::GetInstance();			

	PIDData BlockMovePID = PIDDataManager::GetInstance()->GetPIDData(BlockMovePIDState);

	int currentAngle = SpManager->RobotAngle;
	int ForwardPower = 10;

	int targetWaypointAngle = BtManager->GetDstWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
	int targetBlockAngle = BtManager->GetDstBlockAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);

	switch(SubState){
	// 初回処理
	case Initialize:
		SubState = FirstTurn;
		break;

	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 角度が一致したため、仮想ウェイポイント間の移動に遷移	
		if (abs(targetWaypointAngle - currentAngle) < 2) {
			SubState = ImaginaryWaypoint;
		}

		// 旋回動作を実行
		IoManager->Turn(currentAngle, targetWaypointAngle, TURN_POWER);
		
		break;
	// ウェイポイントに向かって移動する動作
	case ImaginaryWaypoint:
		// ラインを認識した場合には、直進ステートに遷移
		if(IoManager->InputData.ReflectLight < ONLINE) {
			// ラインをまたぐまで直進
			SubState = OverLine;

			// ラインを跨いだので、ウェイポイントの座標に変更
			SpManager->ResetPoint(BtManager->GetLine(BtManager->GetDstWayPointNo())->WayPoint);

		} else {
			IoManager->Forward(ForwardPower);
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
			} else {
				// 初回旋回に遷移
				SubState = FirstTurn;
			}
		} else {	
			// ラインをまたぐまでは直進
			IoManager->Forward(ForwardPower);
		}
		break;
	// ライントレース前の旋回動作
	case LineTurn:
		// 旋回動作を実行
		if(abs(targetBlockAngle - currentAngle) > 45 || IoManager->InputData.ReflectLight > EDGE_LINE){
			IoManager->Turn(currentAngle, targetBlockAngle, TURN_POWER);
		} else {
			// 角度が一致したため、ライントレースに遷移
			SubState = LineTrace;

			// 旋回方向によって、ラインのどちら側を走行するか決定	
			if(targetBlockAngle > currentAngle) LeftEdge = false;
			else LeftEdge = true;
		}
		break;	
	// ライントレースする動作	
	case LineTrace:
		//目標とするブロック置き場の色を取得したら、現在のステートをブロック運搬中に変更
		if(IoManager->HSLKind == BtManager->GetDstBlockPositionColor()){
			// ブロック置き場の座標に修正
			SpManager->ResetPoint(BtManager->GetSrcBlockPoint());
			SpManager->Distance = 200;
			SubState = Back;
			break;
		}
		
		// ライントレース中は、ラインの角度に修正
		{
			int angle = BtManager->GetLine(BtManager->GetDstWayPointNo())->GetAngle(BtManager->GetDstWayPointNo());
			SpManager->ResetAngle(angle);
		}
		IoManager->LineTraceAction(BlockMovePID, EDGE_LINE, LeftEdge);
		break;
	// 後退する動作
	case Back:
		if(SpManager->Distance < 100) {
			// 10cm後退したら、アプローチに遷移する
			auto approachState = new ApproachState(ParentStrategy);
			approachState->CurrentWayPointNo = CurrentWayPointNo;
			ParentStrategy->ChangeState(approachState);
			break;
		}
		IoManager->Back(BlockMovePID.BasePower);
		break;
	}

}

