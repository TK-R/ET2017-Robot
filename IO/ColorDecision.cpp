#include <stdlib.h>
#include "HSLColor.h"
#include "ColorDecision.h"

HSLColor ColorDecision::White(0,0,0);
HSLColor ColorDecision::Red(0,0,0);
HSLColor ColorDecision::Black(0,0,0);
HSLColor ColorDecision::Yellow(0,0,0);
HSLColor ColorDecision::Blue(0,0,0);
HSLColor ColorDecision::Green(0,0,0);

// 色に対する尤度を求める
double ColorDecision::GetLikelihood(HSLColor* base, HSLColor* sensor)
{
    // 各要素ごとの重み
    double hk = 0.3, sk = 0.3, lk = 0.4;
    return hk * 1 / (1 + abs(base->Hue - sensor->Hue)) + 
           sk * 1 / (1 + abs(base->Saturation - sensor->Saturation)) +
           lk * 1 / (1 + abs(base->Luminosity - sensor->Luminosity));
}

// 定義された値に従って、色の推定結果を求める
HSLColorKind ColorDecision::Decision(HSLColor* color)
{
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

