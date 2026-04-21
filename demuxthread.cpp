#include "demuxthread.h"
#include <iostream>

using namespace std;

DemuxThread::DemuxThread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue)
    : m_audio_queue(audio_queue), m_video_queue(video_queue)
{
    cout << "DemuxThread" << endl;
}

DemuxThread::~DemuxThread()
{
    if (m_thread) {
        Stop();
    }
    cout << "~DemuxThread" << endl;
}

int DemuxThread::Init(const char *url)
{
    cout << "url:" << url << endl;
    int ret = 0;
    m_url = url;

    m_ifmt_ctx = avformat_alloc_context();
    ret = avformat_open_input(&m_ifmt_ctx, url, nullptr, nullptr);
    if (ret < 0) {
        av_strerror(ret, m_err2str, sizeof (m_err2str));
        cout << "avformat_open_input:" << m_err2str << endl;
        return -1;
    }

    ret = avformat_find_stream_info(m_ifmt_ctx, nullptr);
    if (ret < 0) {
        av_strerror(ret, m_err2str, sizeof (m_err2str));
        cout << "avformat_find_stream_info:" << m_err2str << endl;
        return -1;
    }

    av_dump_format(m_ifmt_ctx, 0, m_url.c_str(), 0);

    m_audio_index = av_find_best_stream(m_ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    m_video_index = av_find_best_stream(m_ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    if (m_audio_index < 0 || m_video_index < 0)
    {
        cout << "no audio or video" << endl;
        return -1;
    }

    cout << "DemuxThread::Init leave" << endl;

    return 0;
}

int DemuxThread::Start()
{
    m_thread = new std::thread(&DemuxThread::Run, this);
    if (!m_thread) {
        cout << "new std::thread(&DemuxThread::Run, this) failed" << endl;
        return -1;
    }
    return 0;
}

int DemuxThread::Stop()
{
    Thread::Stop();
    avformat_close_input(&m_ifmt_ctx);
    return 0;
}

void DemuxThread::Run()
{
    cout << "DemuxThread::Run into" << endl;
    int ret = 0;
    AVPacket pkt;

    while (m_abort != 1) {

        if (m_audio_queue->Size() > 100 || m_video_queue->Size() > 100) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        ret = av_read_frame(m_ifmt_ctx, &pkt);
        if (ret < 0) {
            av_strerror(ret, m_err2str, sizeof (m_err2str));
            break;
        }

        if (pkt.stream_index == m_audio_index) {
            m_audio_queue->Push(&pkt);

        }
        else if (pkt.stream_index == m_video_index) {
            m_video_queue->Push(&pkt);
            //cout << "video pkt queue size:" << m_video_queue->Size() << endl;
        }
        av_packet_unref(&pkt);
    }

    cout << "DemuxThread::Run leave" << endl;
}

AVCodecParameters *DemuxThread::AudioCodecParameters()
{
    if (m_audio_index != -1)
    {
        return m_ifmt_ctx->streams[m_audio_index]->codecpar;
    }
    else
    {
        return nullptr;
    }
}

AVCodecParameters *DemuxThread::VideoCodecParameters()
{
    if (m_video_index != -1)
    {
        return m_ifmt_ctx->streams[m_video_index]->codecpar;
    }
    else
    {
        return nullptr;
    }
}

AVRational DemuxThread::AudioStreamTimebase()
{
    if (m_audio_index != -1)
    {
        return m_ifmt_ctx->streams[m_audio_index]->time_base;
    }
    else
    {
        return AVRational {0, 0};
    }
}

AVRational DemuxThread::VideoStreamTimebase()
{
    if (m_video_index != -1)
    {
        return m_ifmt_ctx->streams[m_video_index]->time_base;
    }
    else
    {
        return AVRational {0, 0};
    }
}
