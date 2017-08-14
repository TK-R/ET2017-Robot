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
    static const HSLColor Red;
    static const HSLColor Black;
    static const HSLColor Yellow;
    static const HSLColor Blue;
    static const HSLColor Green;

};

#endif