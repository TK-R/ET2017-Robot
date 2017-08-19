#ifndef __COLOR_DICISION__
#define __COLOR_DICISION__

#include  "HSLColor.h"

// 色検知する対象のタイプ
enum DetectTargetType {
    FieldColor = 0,  // 布の床
    ArenaColor,      // 土俵上の床
    BlockColor       // ブロック
};

enum HSLColorKind {
    HSLWhite = 0,
    HSLBlack = 1,
    HSLRed,
    HSLYellow,
    HSLBlue,
    HSLGreen,
};

struct HSLColors {
    HSLColor White;  
    HSLColor Black;
    HSLColor Red;
    HSLColor Yellow;
    HSLColor Blue;
    HSLColor Green;
};

class ColorDecision
{
private:
    static HSLColors FieldColors;
    static HSLColors BlockColors;
    static HSLColors ArenaColors; 

public:
    static double GetLikelihood(HSLColor* base, HSLColor* sensor);
    static HSLColorKind Decision(HSLColor* color, DetectTargetType type);
};

#endif