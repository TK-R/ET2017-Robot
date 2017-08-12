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

HSLColor* FieldMap::GetFieldColor(int x, int y)
{
	return HSLColor::FromRGB(0, 0, 0);
}
