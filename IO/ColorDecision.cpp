#include <stdlib.h>
#include "HSLColor.h"
#include "ColorDecision.h"

// フィールドの床の色認識に使用する基準色
struct HSLColors ColorDecision::FieldColors = 
{
    {300,   5,   100},  // White
    {252,   24,    4},  // Black
    {3,     53,   52},  // Red
    {40,    57,   58},  // Yellow
    {218,   81,   42},  // Blue
    {125,   56,   35}   // Green
};

// ブロックの色認識に使用する基準色
struct HSLColors ColorDecision::BlockColors = 
{
    {300,   5,   100},  // White
    {252,   24,    4},  // Black
    {5,     82,   35},  // Red
    {44,    86,   33},  // Yellow
    {190,   73,   16},  // Blue
    {122,   63,   10}   // Green
};

// 土俵の床の色認識に使用する基準色
struct HSLColors ColorDecision::ArenaColors = 
{
    {300,   5,   100},  // White
    {252,   24,    4},  // Black
    {3,     53,   52},  // Red
    {40,    57,   58},  // Yellow
    {218,   81,   42},  // Blue
    {125,   56,   35}   // Green
};

// 色に対する尤度を求める
double ColorDecision::GetLikelihood(HSLColor* base, HSLColor* sensor)
{
    // 各要素ごとの重み
    double hk = 10, sk = 1, lk = 0.5;
    double hueDiff = abs(base->Hue - sensor->Hue);
    if(hueDiff > 180) hueDiff = 360 - hueDiff;

    return hk * 1 / (1 + abs(base->Hue - sensor->Hue)) + 
           sk * 1 / (1 + abs(base->Saturation - sensor->Saturation)) +
           lk * 1 / (1 + abs(base->Luminosity - sensor->Luminosity));
}

// 明度に対する尤度を求める
double ColorDecision::GetLikelihoodLuminosity(double lumiBase, double lumiSens)
{   
    // 明るさに応じて、尤度を計算する
    return 1.0 / (1.0 + abs(lumiBase - lumiSens));
}

// 定義された値に従って、色の推定結果を求める
HSLColorKind ColorDecision::Decision(HSLColor* color, DetectTargetType type)
{
    // 輝度が100の場合には、間違いなく白
    if(color->Luminosity >= 100) {
        return HSLWhite;
    }

    HSLColors colors;
    switch(type) { 
        case FieldColor:
            colors = FieldColors;
            break;
        case BlockColor:
            colors = BlockColors;
            break;
        case ArenaColor:
            colors = ArenaColors;
            break;
    }

    double bestLikelihood = 0, tmpLikelihood; 
    HSLColorKind kind;

    // 白を判定
    bestLikelihood = GetLikelihood(&colors.White, color);
    // 最初の判定
    kind = HSLWhite; 

    // 黒を判定
    tmpLikelihood = GetLikelihood(&colors.Black, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLBlack;
    }

    // 赤を判定
    tmpLikelihood = GetLikelihood(&colors.Red, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLRed;
    }

    // 黄を判定
    tmpLikelihood = GetLikelihood(&colors.Yellow, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLYellow;
    }

    // 青を判定
    tmpLikelihood = GetLikelihood(&colors.Blue, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLBlue;
    }

    // 緑を判定
    tmpLikelihood = GetLikelihood(&colors.Green, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLGreen;
    }

    return kind;
}

