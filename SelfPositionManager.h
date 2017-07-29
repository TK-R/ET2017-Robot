#ifndef SELF_POSITION_MANAGER_
#define SELF_POSITION_MANAGER_



class SelfPositionManager
{
	private:
	public:
		double PositionX, PositionY, Angle;
		SelfPositionManager(SelfPositionData p);

		void UpdatePosition(int8_t leftMotorCount, int8_t rightMotorCount);
		void ResetPosition(SelfPositionData p);
};



#endif