#include <stdio.h>
#include <string.h>
#include "ev3api.h"
#include "HSLColor.h"
#include "FieldMap.h"

// C++からC言語ライブラリをインクルードするため
extern "C"{
	#include "image.h"
}

bool FieldMap::ReadImage(const char* fileName)
{	
	// 読み込み済みのため、解放してから読み込み処理を実行
	if(Image != NULL)
		free_image(Image);

	Image = read_bmp_simple_file(fileName);
	return true;
}

FieldMap* FieldMap::GetInstance()
{
	static FieldMap map;
	return &map;
}

HSLColor FieldMap::GetHSLColor(int x, int y)
{
	RGBColor rgb = GetRGBColor(x, y);
	HSLColor hsl = HSLColor::FromRGB(rgb.R, rgb.G, rgb.B);

	return hsl;
}

RGBColor FieldMap::GetRGBColor(int x, int y)
{
	// 縮尺を計算(1/10)
	x /=10;
	y /=10;

	// 画像の範囲外は縁のデータを返す
	if(x > (int)Image->width)
		x = Image->width;
	else if(x < 0)
		x = 0;
	
	if(y > (int)Image->height) 
		y = Image->height;
	else if(y < 0)
		y = 0;

	RGBColor color;
	color.R = Image->map[y][x].c.r;
    color.G = Image->map[y][x].c.g;
	color.B = Image->map[y][x].c.b;


	return color;
}
