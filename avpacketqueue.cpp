#include "avpacketqueue.h"
#include <iostream>

AVPacketQueue::AVPacketQueue()
{

}

AVPacketQueue::~AVPacketQueue()
{
    //Abort();
}

void AVPacketQueue::Abort()
{
    release();
    m_queue.Abort();
}

int AVPacketQueue::Size()
{
    return m_queue.Size();
}

int AVPacketQueue::Push(AVPacket *val)
{
    AVPacket *tmp_pkt = av_packet_alloc();
    av_packet_move_ref(tmp_pkt, val);
    return m_queue.Push(tmp_pkt);
}

AVPacket *AVPacketQueue::Pop(const int timeout)
{
    AVPacket *tmp_pkt = nullptr;
    int ret = m_queue.Pop(tmp_pkt, timeout);
    if (ret < 0) {
        if (-1 == ret)
            std::cout << "AVPacketQueue::Pop failed" << std::endl;
    }

    return tmp_pkt;
}

void AVPacketQueue::release()
{
    while (true) {
        AVPacket *packet = nullptr;
        int ret = m_queue.Pop(packet, 1);
        if (ret == 0) continue;
        else break;
    }
}
