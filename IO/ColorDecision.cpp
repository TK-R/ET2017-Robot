#include <stdlib.h>
#include "HSLColor.h"
#include "ColorDecision.h"

HSLColor ColorDecision::White   (300,   5,   100);
HSLColor ColorDecision::Red     (3,     53,   52);
HSLColor ColorDecision::Black   (252,   24,    4);
HSLColor ColorDecision::Yellow  (40,    57,   58);
HSLColor ColorDecision::Blue    (218,   81,   42);
HSLColor ColorDecision::Green   (125,   56,   35);

// 色に対する尤度を求める
double ColorDecision::GetLikelihood(HSLColor* base, HSLColor* sensor)
{
    // 各要素ごとの重み
    double hk = 0.4, sk = 0.3, lk = 0.3;
    return hk * 1 / (1 + abs(base->Hue - sensor->Hue)) + 
           sk * 1 / (1 + abs(base->Saturation - sensor->Saturation)) +
           lk * 1 / (1 + abs(base->Luminosity - sensor->Luminosity));
}

// 定義された値に従って、色の推定結果を求める
HSLColorKind ColorDecision::Decision(HSLColor* color)
{
    // 輝度が100の場合には、間違いなく白
    if(color->Luminosity >= 100) {
        return HSLWhite;
    }

    double bestLikelihood = 0, tmpLikelihood; 
    HSLColorKind kind;

    // 白を判定
    bestLikelihood = GetLikelihood(&White, color);
    // 最初の判定
    kind = HSLWhite; 

    // 黒を判定
    tmpLikelihood = GetLikelihood(&Black, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLBlack;
    }

    // 赤を判定
    tmpLikelihood = GetLikelihood(&Red, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLRed;
    }

    // 黄を判定
    tmpLikelihood = GetLikelihood(&Yellow, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLYellow;
    }

    // 青を判定
    tmpLikelihood = GetLikelihood(&Blue, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLBlue;
    }

    // 緑を判定
    tmpLikelihood = GetLikelihood(&Green, color);
    if(tmpLikelihood > bestLikelihood){
        bestLikelihood = tmpLikelihood;
        kind = HSLGreen;
    }

    return kind;
}

