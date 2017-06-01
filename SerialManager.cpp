#include "ev3api.h"
#include "app.h"
#include "SerialManager.h"
#include <string.h>
#include <syssvc/serial.h>

void BluetoothTask(intptr_t exinf)
{

//	FILE *bt = ev3_serial_open_file(EV3_SERIAL_BT);
//   fprintf(bt, "\r\nEcho test is started.\r\n");

	while(true)
	{
		if (!ev3_bluetooth_is_connected())
			continue;

		Header header;
		header.Head = 0xff;
		header.Size = 0x3;
		header.Command = 0x02;

		OutputSignalData data;
		data.MovePower = 1;
		data.Steering = 2;
		data.Motor3Power = 3;
		
		char buff[100];
		memcpy(buff, &header, 4);
		memcpy(buff + 4, &data, 3);
		buff[7] = 6;

		serial_wri_dat(SIO_PORT_BT, buff, 8);
//		fprintf(bt, "\r\nEcho test is started.\r\n");

		tslp_tsk(100);
	}
}