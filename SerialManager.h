#ifndef __SERIAL_MANAGER_H__
#define __SERIAL_MANAGER_H__

#pragma pack(push, 1) // アライメント1に設定

struct Header {
	uint8_t Head;
	uint16_t Size;
	uint8_t Command;
};

struct OutputSignalData {
	int8_t MovePower;
	int8_t Steering;
	int8_t Motor3Power;	
};
#pragma pack(pop)

#endif