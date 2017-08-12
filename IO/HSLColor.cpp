#include <algorithm>
#include "ev3api.h"
#include "HSLColor.h"
using namespace std;

HSLColor* HSLColor::FromRGB(uint8_t R, uint8_t G, uint8_t B)
{
	float red = (R / 255.0);
	float green = (G / 255.0);
	float blue = (B / 255.0);

	float _Min = min(min(red, green), blue);
	float _Max = max(max(red, green), blue);
	float _Delta = _Max - _Min;

	float H = 0.0;
	float S = 0.0;
	float L = (float)((_Max + _Min) / 2.0);

	if (_Delta != 0)
	{
		if (L < 0.5)
		{
			S = (float)(_Delta / (_Max + _Min));
		}
		else
		{
			S = (float)(_Delta / (2.0 - _Max - _Min));
		}


		if (red == _Max)
		{
			H = (green - blue) / _Delta;
		}
		else if (green == _Max)
		{
			H = 2.0 + (blue - red) / _Delta;
		}
		else if (blue == _Max)
		{
			H = 4.0 + (red - green) / _Delta;
		}
	}

	return new HSLColor(H, S, L);
}

