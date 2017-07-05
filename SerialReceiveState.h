#ifndef __SERIAL_STATE__
#define __SERIAL_STATE__

class SerialManager;

class SerialState
{
protected:
	SerialManager* Context;
//s	protected List<byte> buff;

	SerialState(SerialManager* manager)
	{
		Context = manager;
	}

 	virtual void Receive(uint8_t data) {}

};

class HeaderState : SerialState
{
	virtual void Receive(uint8_t data);
};

class HeaderDataState : SerialState
{
	virtual void Receive(uint8_t data);
};

class OutputDataState : SerialState
{
	virtual void Receive(uint8_t data);
};

class InputDataState : SerialState
{
	virtual void Receive(uint8_t data);
};

class PIDDataState : SerialState
{
	virtual void Receive(uint8_t data);
};

#endif