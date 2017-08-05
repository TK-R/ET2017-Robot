#ifndef __COLOR_DICISION__
#define __COLOR_DICISION__

#include  "HSLColor.h"

enum HSLColorKind {
    HSLRed,
    HSLBlack,
    HSLYellow,
    HSLBlue,
    HLGreen,
};

class ColorDicision
{
public:
    static HSLColorKind Decision(HSLColor color);
private:
    HSLColor* Red;
    HSLColor* Black;
    HSLColor* Yellow;
    HSLColor* Blue;
    HSLColor* Green;
};

#endif