#include <algorithm>
#include "ev3api.h"
#include "HSLColor.h"
using namespace std;

HSLColor HSLColor::FromRGB(uint8_t R, uint8_t G, uint8_t B)
{
//	float red = (R / 255.0);
//	float green = (G / 255.0);
//	float blue = (B / 255.0);

	float red = R;
	float green = G;
	float blue = B;

	float _Min = min(min(red, green), blue);
	float _Max = max(max(red, green), blue);
	float _Delta = _Max - _Min;

	float H = 0.0;
	float S = 0.0;
	float L = 0.0;
	float CNT = (float)((_Max + _Min) / 2.0);

	HSLColor value;

	if (_Delta != 0) {
		if (CNT < 128) {
			S = (float)(_Delta / (_Max + _Min));
		} else {
			S = (float)(_Delta / (510 -	CNT));
		}

		if (red == _Max) {
			H = 60 * (green - blue) / _Delta;
		} else if (green == _Max) {
			H = 60 * (blue - red) / _Delta + 120;
		} else if (blue == _Max) {
			H = 60 * (red - green) / _Delta + 240;
		}
	}

	L = CNT;

	value.Hue = H;
	value.Saturation = S * 100;
	value.Luminosity = L / 2.55;
	return value;
}

void HSLColor::FromRGB(uint8_t R, uint8_t G, uint8_t B, HSLColor* OutColor)
{
	HSLColor c = FromRGB(R, G, B);
	OutColor->Hue = c.Hue;
	OutColor->Saturation = c.Saturation;
	OutColor->Luminosity = c.Luminosity;
}

