#include "audiooutput.h"
#include <iostream>

AudioOutput::AudioOutput(AVSync *avsync, AVRational time_base, const AudioParams &audio_params, AVFrameQueue *frame_queue)
    : m_avsync (avsync), m_time_base (time_base),
      m_src_params(audio_params), m_frame_queue(frame_queue)
{

}

AudioOutput::~AudioOutput()
{

}

void fill_audio_pcm(void *udata, Uint8 *stream, int len)
{
    AudioOutput *is = (AudioOutput *)udata;
    int len1 = 0;
    int audio_size = 0;
    while (len > 0) {
        if (is->m_audio_buf_index == is->m_audio_buf_size) {
            is->m_audio_buf_index = 0;
            AVFrame *frame = is->m_frame_queue->Pop(10);
            is->m_pts = frame->pts;
            if (frame) {
                if (((frame->format != is->m_dst_params.fmt) || (frame->sample_rate != is->m_dst_params.freq)
                     || (frame->channel_layout != is->m_dst_params.channel_layout)) && (!is->m_swr_ctx)) {
                    is->m_swr_ctx = swr_alloc_set_opts(nullptr,
                                                       is->m_dst_params.channel_layout,
                                                       (enum AVSampleFormat) is->m_dst_params.fmt,
                                                       is->m_dst_params.freq,
                                                       frame->channel_layout,
                                                       (enum AVSampleFormat) frame->format,
                                                       frame->sample_rate,
                                                       0, nullptr);
                    if (!is->m_swr_ctx || swr_init(is->m_swr_ctx) < 0) {
                         std::cout << "重采样初始化失败！" << std::endl;
                        swr_free(&is->m_swr_ctx);
                        return;
                    } else {
                        std::cout << "重采样初始化成功" << std::endl;
                    }
                }
                if (is->m_swr_ctx) {
                    const uint8_t **in = (const uint8_t **)frame->extended_data;
                    uint8_t **out = &is->m_audio_buf1;
                    int out_samples = frame->nb_samples * is->m_dst_params.freq / frame->sample_rate + 256;
                    int out_bytes = av_samples_get_buffer_size(nullptr, is->m_dst_params.channels, out_samples, is->m_dst_params.fmt, 0);
                    if (out_bytes < 0) {
                        std::cout << "av_samples_get_buffer_size failed" << std::endl;
                        return;
                    }
                    av_fast_malloc(&is->m_audio_buf1, &is->m_audio_buf1_size, out_bytes);
                    int len2 = swr_convert(is->m_swr_ctx, out, out_samples, in, frame->nb_samples);
                    if (len2 < 0) {
                        std::cout << "swr_convert failed" << std::endl;
                    }
                    is->m_audio_buf = is->m_audio_buf1;
                    is->m_audio_buf_size = av_samples_get_buffer_size(nullptr, is->m_dst_params.channels, len2, is->m_dst_params.fmt, 1);
                } else {
                    audio_size = av_samples_get_buffer_size(nullptr, frame->channels, frame->nb_samples, (enum AVSampleFormat)frame->format, 1);
                    av_fast_malloc(&is->m_audio_buf1, &is->m_audio_buf1_size, audio_size);
                    is->m_audio_buf = is->m_audio_buf1;
                    is->m_audio_buf_size = audio_size;
                    memcpy(is->m_audio_buf, frame->data[0], audio_size);
                }
                av_frame_free(&frame);
            } else {
                is->m_audio_buf_size = 512;
                is->m_audio_buf = nullptr;
            }
        }
        len1 = is->m_audio_buf_size - is->m_audio_buf_index;
        if (len1 > len) {
            len1 = len;
        }
        if (!is->m_audio_buf) {
            memset(stream, 0, len1);
        } else {
            memcpy(stream, is->m_audio_buf + is->m_audio_buf_index, len1);
        }
        len -= len1;
        stream += len1;
        is->m_audio_buf_index += len1;
    }
    if (is->m_pts != AV_NOPTS_VALUE) {
        double pts = is->m_pts * av_q2d(is->m_time_base);

        //std::cout << "audio pts:" << pts << std::endl;
        is->m_avsync->SetClock(pts);
    }

}

int AudioOutput::Init()
{
    std::cout << "AudioOutput::Init into" << std::endl;
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL_Init failed" << std::endl;
        return -1;
    }
    SDL_AudioSpec want_spec{}, spec{};
    want_spec.channels = m_src_params.channels;
    want_spec.freq = m_src_params.freq;
    want_spec.format = AUDIO_S16SYS;
    want_spec.silence = 0;
    want_spec.callback = fill_audio_pcm;
    want_spec.userdata = this;
    want_spec.samples = 1024;
    std::cout << "want_spec参数: 声道数=" << (int)want_spec.channels
              << " 采样率=" << want_spec.freq
              << std::dec << std::endl;

    int ret = SDL_OpenAudio(&want_spec, &spec);
    if (ret < 0) {
        std::cout << "SDL_OpenAudio failed" << std::endl;
        return -1;
    }
    m_dst_params.channels = spec.channels;
    //m_dst_params.fmt = AV_SAMPLE_FMT_S16;
    if (spec.format == AUDIO_S16SYS) {
            m_dst_params.fmt = AV_SAMPLE_FMT_S16;
        } else if (spec.format == AUDIO_F32SYS) {
            m_dst_params.fmt = AV_SAMPLE_FMT_FLT;
        } // ... 其他格式判断
    m_dst_params.freq = spec.freq;
    m_dst_params.channel_layout = av_get_default_channel_layout(m_dst_params.channels);
    m_dst_params.frame_size = spec.samples;

    // 打印校验（调试用）
    std::cout << "SDL实际参数: 声道数=" << (int)spec.channels
              << " 采样率=" << spec.freq
              << " 布局=0x" << std::hex << m_dst_params.channel_layout
              << " spec.format=" << spec.format << std::dec << std::endl;

    SDL_PauseAudio(0);
    std::cout << "AudioOutput::Init leave" << std::endl;

    return 0;
}

int AudioOutput::DeInit()
{
    std::cout << "AudioOutput::DeInit into" << std::endl;
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    std::cout << "AudioOutput::DeInit leave" << std::endl;
}
