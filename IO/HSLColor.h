#ifndef __HSLCOLOR__
#define __HSLCOLOR__

#include "ev3api.h"

class HSLColor { 
public: 
	float Hue;
	float Saturation;
    float Luminosity;

	HSLColor(){};

    HSLColor(float H, float S, float L)
	{
    	Hue = H;
        Saturation = S;
        Luminosity = L;
    };

	static HSLColor FromRGB(uint8_t R, uint8_t G, uint8_t B);
	static void FromRGB(uint8_t R, uint8_t G, uint8_t B, HSLColor* OutColor);

};

class RGBColor
{
public:
	int R;
	int G;
	int B;

	RGBColor(){};

	RGBColor(int r, int g, int b)
	{
		R = r;
		G = g;
		B = b;
	}
};

#endif 