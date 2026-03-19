/**
 * @file modded_bgm.h
 * @brief Modded background music and voice line API.
 */
#ifndef MODDED_BGM_H
#define MODDED_BGM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void ModdedBGM_Init(void);
void ModdedBGM_Exit(void);

/**
 * @brief Attempts to play a modded BGM track for the given file_id.
 *
 * Checks if modded BGM is enabled, and if a corresponding file
 * (e.g. 89.ogg, 89.mp3, 89.wav) exists in the assets/bgm_mod/ folder.
 *
 * Loop points: For OGG files, embed LOOPSTART/LOOPLENGTH vorbis comment
 * tags — SDL3_mixer handles them natively. For other formats, place a
 * sidecar file (e.g. 89.loop) with LOOPSTART=N and LOOPLENGTH=N lines.
 *
 * @param file_id The track ID (fnum) to attempt to play.
 * @return true if a modded track was found and playback started successfully.
 *         false if no modded track was found or modding is disabled.
 */
bool ModdedBGM_Play(int file_id);

void ModdedBGM_Stop(void);
void ModdedBGM_SetVolume(int volume_db10);
void ModdedBGM_Pause(bool pause);

/**
 * @brief Fade out the currently playing modded BGM over the given duration.
 * @param fade_ms Duration of fade in milliseconds. 0 = immediate stop.
 */
void ModdedBGM_FadeOut(int fade_ms);

/**
 * @brief Attempts to play a modded voice line (non-looping, one-shot).
 *
 * Looks for voice_mod/{voice_name}.ogg (or .flac/.opus/.mp3/.wav)
 * in the assets folder. Used for announcer / VS screen / char select clips.
 *
 * @param voice_name The voice identifier (e.g. "vs", "emsel").
 * @return true if a modded voice file was found and playback started.
 */
bool ModdedBGM_PlayVoice(const char* voice_name);

/**
 * @brief Check if a modded voice file exists for the given name.
 */
bool ModdedBGM_IsVoiceModded(const char* voice_name);

/**
 * @brief Count how many modded BGM files are detected in assets/bgm_mod/.
 * @return Number of audio files found (any supported extension).
 */
int ModdedBGM_CountModdedTracks(void);

#ifdef __cplusplus
}
#endif

#endif // MODDED_BGM_H
