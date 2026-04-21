#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H

#include "thread.h"
#include "avpacketqueue.h"

#ifdef __cplusplus
extern "C"
{
    //包含ffmpeg头文件
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}
#endif


class DemuxThread : public Thread
{
public:
    DemuxThread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue);
    ~DemuxThread();

    int Init(const char *url);
    int Start();
    int Stop();
    void Run();

    AVCodecParameters *AudioCodecParameters();
    AVCodecParameters *VideoCodecParameters();

    AVRational AudioStreamTimebase();
    AVRational VideoStreamTimebase();

private:
    char m_err2str[256];
    std::string m_url;
    AVPacketQueue *m_audio_queue;
    AVPacketQueue *m_video_queue;

    AVFormatContext *m_ifmt_ctx = nullptr;

    int m_audio_index = -1;
    int m_video_index = -1;
};

#endif // DEMUXTHREAD_H
