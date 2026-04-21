#include "avframequeue.h"
#include <iostream>

AVFrameQueue::AVFrameQueue()
{

}

AVFrameQueue::~AVFrameQueue()
{
    //Abort();
}

void AVFrameQueue::Abort()
{
    release();
    m_queue.Abort();
}

AVFrame *AVFrameQueue::Front()
{
    AVFrame *tmp_frame = NULL;
    int ret = m_queue.Front(tmp_frame);
    if (ret < 0) {
        if (-1 == ret) {
             std::cout << ("AVFrameQueue::Front failed") << std::endl;
        }
    }
    return tmp_frame;
}

int AVFrameQueue::Size()
{
    return m_queue.Size();
}

int AVFrameQueue::Push(AVFrame *var)
{
    AVFrame *tmp_frame = av_frame_alloc();
    av_frame_move_ref(tmp_frame, var);
    return m_queue.Push(tmp_frame);
}

AVFrame *AVFrameQueue::Pop(const int timeout)
{
    AVFrame *tmp_frame = nullptr;
    int ret = m_queue.Pop(tmp_frame, timeout);
    if (ret < 0) {
        if (-1 == ret)
            std::cout << "AVFrameQueue::Pop failed" << std::endl;
    }

    return tmp_frame;
}

void AVFrameQueue::release()
{
    while (true) {
        AVFrame *tmp_frame = nullptr;
        int ret = m_queue.Pop(tmp_frame, 1);
        if (ret == 0) continue;
        else break;
    }
}
