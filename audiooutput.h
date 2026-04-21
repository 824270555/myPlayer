#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H
#include "avframequeue.h"
#include "avsync.h"

#ifdef __cplusplus
extern "C" {
#include "SDL.h"
#include "libswresample/swresample.h"
#include "libavutil/time.h"
}
#endif

typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
} AudioParams;

class AudioOutput
{
public:
    AudioOutput(AVSync *avsync, AVRational time_base, const AudioParams &audio_params, AVFrameQueue *frame_queue);
    ~AudioOutput();

    int Init();
    int DeInit();

public:
    AVSync *m_avsync = nullptr;
    AVRational m_time_base;

    int64_t m_pts = AV_NOPTS_VALUE;
    AudioParams m_src_params;
    AudioParams m_dst_params;
    AVFrameQueue *m_frame_queue = nullptr;

    struct SwrContext *m_swr_ctx = nullptr;
    uint8_t *m_audio_buf = nullptr;
    uint8_t *m_audio_buf1 = nullptr;

    uint32_t m_audio_buf_size = 0;
    uint32_t m_audio_buf1_size = 0;
    uint32_t m_audio_buf_index = 0;


};

#endif // AUDIOOUTPUT_H
