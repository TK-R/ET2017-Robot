#include "ev3api.h"
#include "SoundPlayTask.h"
#include "app.h"

//#define SILENT_MODE 1

void Play(const char* fileName)
{
    memfile_t mem;
    ev3_memfile_load(fileName, &mem);
    ev3_speaker_set_volume(10);
    ev3_speaker_play_file(&mem, SOUND_MANUAL_STOP);
    ev3_memfile_free(&mem);
}

// 音声再生タスク
void sound_play_task(intptr_t exinf)
{
    intptr_t id;
    SoundType type;
    while(1){
        rcv_dtq(DT_QID, &id);
        type = (SoundType)id;
        switch(type)
        {
            case SensorInitialStart:
                Play("/ev3rt/sound/S01.wav");
                break;
            case SensorInitialEnd:
                Play("/ev3rt/sound/S02.wav");
                break;
            case LCourseStart:
                Play("/ev3rt/sound/S03.wav");
                break;
            case RCourseStart:
                Play("/ev3rt/sound/S04.wav");
                break;
            case BlockEnd:
                Play("/ev3rt/sound/S05.wav");
                break;
            case RuleReceive:
                Play("/ev3rt/sound/S06.wav");
                break;        
            case PIDReceive:
                Play("/ev3rt/sound/S07.wav");
                break;
        }
    }
}

void PlaySound(SoundType s)
{
#ifndef SILENT_MODE
    snd_dtq(DT_QID, s);
#endif
}