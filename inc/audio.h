#ifndef AUDIO_H_    // include guard
#define AUDIO_H_

// c++ guard
#ifdef __cplusplus
extern "C" {
#endif

#include <pspuser.h>

#define PBP_SIGNATURE   0x50425000
#define RIFF_SIGNATURE  0x46464952

typedef struct {
    bool loop;
    int size;
    int index;
    u32 *data;
} Wav;

typedef  struct {
    int channel;
} AudioData;

void audioCallback(void* buf, unsigned int length, int channel);
void initMusic();
Wav * loadWav(char *path);
void playWav(Wav *wav, int priority);

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif  // AUDIO_H_
