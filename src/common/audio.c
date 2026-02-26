#include "audio.h"

#include <pspkernel.h>
#include <pspuser.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspatrac3.h>
#include <malloc.h>

#define printf(...) pspDebugScreenPrintf(__VA_ARGS__)

#define AUDIO_BG_CHANNEL   0
#define AUDIO_SAMPLES   1024

Wav *playChannels[PSP_AUDIO_CHANNEL_MAX];

void audioCallback(void* buf, unsigned int length, int channel) {
    int i;
    int *s = (int *) buf;
    Wav *play = playChannels[channel];

    for(i = 0; i < length; i++){
        if(play == NULL)
            s[i] = 0;
        else{
            s[i] = play->data[play->index];
            if(i % 2)
                play->index += 1;
            if(play->index == play->size){
                play->index = 0;
                playChannels[channel] = NULL;
                play = NULL;
            }
        }
    }
}

void audioCallback_0(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 0); }
void audioCallback_1(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 1); }
void audioCallback_2(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 2); }
void audioCallback_3(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 3); }
void audioCallback_4(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 4); }
void audioCallback_5(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length,5); }
void audioCallback_6(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 6); }
void audioCallback_7(void *buf, unsigned int length, void *userdata){ audioCallback(buf, length, 7); }

void initMusic(){
    int i;

    pspAudioInit();
    for(i = 0; i < PSP_AUDIO_CHANNEL_MAX; i++){
        playChannels[i] = NULL;
    }
    pspAudioSetChannelCallback(0, audioCallback_0, NULL);
    pspAudioSetChannelCallback(1, audioCallback_1, NULL);
    pspAudioSetChannelCallback(2, audioCallback_2, NULL);
    pspAudioSetChannelCallback(3, audioCallback_3, NULL);
    pspAudioSetChannelCallback(4, audioCallback_3, NULL);
    pspAudioSetChannelCallback(5, audioCallback_3, NULL);
    pspAudioSetChannelCallback(6, audioCallback_3, NULL);
    pspAudioSetChannelCallback(7, audioCallback_3, NULL);


    for(i = 0; i < PSP_AUDIO_CHANNEL_MAX; i++){
        pspAudioSetVolume(i, PSP_VOLUME_MAX, PSP_VOLUME_MAX);
    }
}

Wav * loadWav(char *path){
    Wav *r = (Wav*) calloc(1, sizeof(Wav));
    char s;
    
    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

    if(fd < 0)
        return NULL;

    sceIoLseek(fd, 40, PSP_SEEK_SET);
    sceIoRead(fd, &r->size, sizeof(r->size));

    r->data = memalign(16, r->size);
    printf("%x\n", r->data);
    if(r->data)
        sceIoRead(fd, r->data, r->size);

    sceIoClose(fd);

    r->size /= sizeof(u32);

    return r;
}

void playWav(Wav *wav, int priority){
    int i;
    Wav *play = NULL;
    for(i = 1; i < PSP_AUDIO_CHANNEL_MAX; i++){
        if(playChannels[i] = wav){
            playChannels[i]->index = 0;
            break;
        }
    }
    if(i == PSP_AUDIO_CHANNEL_MAX)
        playChannels[priority % PSP_AUDIO_CHANNEL_MAX] = wav;
}