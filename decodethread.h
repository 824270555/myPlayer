#ifndef DECODETHREAD_H
#define DECODETHREAD_H
#include <thread.h>
#include "avpacketqueue.h"
#include "avframequeue.h"

class DecodeThread : public Thread
{
public:
    DecodeThread(AVPacketQueue *packet_queue, AVFrameQueue *frame_queue);
    ~DecodeThread();

    int Init(AVCodecParameters *par);
    int Start();
    int Stop();
    void Run();

private:
    char err2str[256];
    AVCodecContext *m_codec_ctx = nullptr;
    AVPacketQueue *m_packet_queue = nullptr;
    AVFrameQueue *m_frame_queue = nullptr;
};

#endif // DECODETHREAD_H
