#ifndef __SERIAL_STATE__
#define __SERIAL_STATE__

#include "ev3api.h"
#include <vector>

class SerialManager;

class SerialState
{
protected:
	SerialManager* Context;
	std::vector<uint8_t> buff;

public:
	SerialState(SerialManager* manager)
	{
		Context = manager;
	}

 	virtual void Receive(uint8_t data) { syslog(0, "ERROR CALL..."); }
	virtual ~SerialState(){}
};

class HeaderState : public SerialState
{
public:
	virtual void Receive(uint8_t data);
	using SerialState::SerialState;
};

class HeaderDataState : public SerialState
{
public:
	virtual void Receive(uint8_t data);
	HeaderDataState(SerialManager* manager) : SerialState(manager)
	{
		// ヘッダコードを追記
		buff.push_back(0xff);
	}
};

class OutputDataState : public SerialState
{
public:
	virtual void Receive(uint8_t data);
	using SerialState::SerialState;
};

class InputDataState : public SerialState
{
public:
	virtual void Receive(uint8_t data);
	using SerialState::SerialState;
};

class PIDDataState : public SerialState
{
public:
	virtual void Receive(uint8_t data);
	using SerialState::SerialState;

};

#endif