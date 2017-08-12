#ifndef __FIELD_MAP__
#define __FIELD_MAP__

#include "HSLColor.h"

extern "C"{
	#include "image.h"
}

class FieldMap
{
public:
	bool ReadImage(const char* fileName);
	static FieldMap* GetInstance();
	HSLColor* GetFieldColor(int x, int y);

private:
	imageData_t* Image = NULL;
};

#endif