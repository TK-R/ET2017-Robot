#include "ev3api.h"
#include "SoundPlayTask.h"


// 音声再生タスク
void sound_play_task(intptr_t exinf)
{
    const char* path = "/ev3rt/sound/S_01.wav";

    memfile_t mem;
    ev3_memfile_load(path, &mem);
    ev3_speaker_set_volume(10);
    ev3_speaker_play_file(&mem, SOUND_MANUAL_STOP);

    ext_tsk();
}

void PlaySound(const char* fileName)
{

}