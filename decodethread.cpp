#include "decodethread.h"
#include <iostream>

DecodeThread::DecodeThread(AVPacketQueue *packet_queue, AVFrameQueue *frame_queue)
    : m_packet_queue(packet_queue), m_frame_queue(frame_queue)
{

}

DecodeThread::~DecodeThread()
{
    if (m_thread)
        Stop();

    if (m_codec_ctx)
        avcodec_close(m_codec_ctx);
}

int DecodeThread::Init(AVCodecParameters *par)
{
    if (!par) {
        std::cout << "Init par is null" << std::endl;
        return -1;
    }

    m_codec_ctx = avcodec_alloc_context3(nullptr);

    int ret = avcodec_parameters_to_context(m_codec_ctx, par);
    if (ret < 0) {
        av_strerror(ret, err2str, sizeof (err2str));
        std::cout << err2str << std::endl;
        return -1;
    }

    const AVCodec *codec = avcodec_find_decoder(m_codec_ctx->codec_id);
    if (ret < 0) {
        av_strerror(ret, err2str, sizeof (err2str));
        std::cout << err2str << std::endl;
        return -1;
    }

    ret = avcodec_open2(m_codec_ctx, codec, nullptr);
    if (ret < 0) {
        av_strerror(ret, err2str, sizeof (err2str));
        std::cout << err2str << std::endl;
        return -1;
    }
    std::cout << "DecodeThread::Init finish" << std::endl;
    return 0;
}

int DecodeThread::Start()
{
    m_thread = new std::thread(&DecodeThread::Run, this);

    if (!m_thread)
        std::cout << "new std::thread(&DecodeThread::Run, this) failed";
}

int DecodeThread::Stop()
{
    Thread::Stop();
}

void DecodeThread::Run()
{
    std::cout << "DecodeThread::Run into" << std::endl;
    AVFrame *frame = av_frame_alloc();
    while (!m_abort) {
        if (m_frame_queue->Size() > 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        AVPacket *pkt = m_packet_queue->Pop(10);

        if (pkt) {
            int ret = avcodec_send_packet(m_codec_ctx, pkt);
            if (ret < 0) {
                av_strerror(ret, err2str, sizeof (err2str));
                std::cout << err2str << std::endl;
                break;
            }
            av_packet_free(&pkt);

            while (true) {
                ret = avcodec_receive_frame(m_codec_ctx, frame);
                if (ret == 0) {
                    m_frame_queue->Push(frame);
                    //std::cout << m_codec_ctx->codec->name << " frame queue size:" << m_frame_queue->Size() << std::endl;
                }
                else if (ret == AVERROR(EAGAIN)) {
                    break;
                }
                else
                {
                    m_abort = 1;
                    av_strerror(ret, err2str, sizeof (err2str));
                    std::cout << err2str << std::endl;
                    break;
                }
            }
        }
        else {

        }
    }

    std::cout << "DecodeThread::Run finish" << std::endl;
}
