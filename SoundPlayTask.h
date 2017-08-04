#ifndef __SOUNDPLAY__
#define __SOUNDPLAY__


// 音声再生タスク

enum SoundType
{
	SensorInitialStart = 0,
	SensorInitialEnd,
	Start,
	LineTraceEnd
};

extern void PlaySound(SoundType s);


#endif