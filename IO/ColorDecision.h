#ifndef __COLOR_DICISION__
#define __COLOR_DICISION__

#include  "HSLColor.h"



enum HSLColorKind {
    HSLWhite = 0,
    HSLBlack = 1,
    HSLRed,
    HSLYellow,
    HSLBlue,
    HSLGreen,
};


class ColorDecision
{
private:
    static HSLColor White;
    static HSLColor Red;
    static HSLColor Black;
    static HSLColor Yellow;
    static HSLColor Blue;
    static HSLColor Green;

public:
    static double GetLikelihood(HSLColor* base, HSLColor* sensor);
    static HSLColorKind Decision(HSLColor* color);
};

#endif