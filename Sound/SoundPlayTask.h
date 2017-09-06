#ifndef __SOUNDPLAY__
#define __SOUNDPLAY__


// 音声再生タスク

enum SoundType
{
	SensorInitialStart = 0,
	SensorInitialEnd,
	LCourseStart,
	RCourseStart,
	BlockEnd
};

extern void PlaySound(SoundType s);


#endif