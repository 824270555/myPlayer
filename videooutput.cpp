#include "videooutput.h"
#include <iostream>
#include <thread.h>

VideoOutPut::VideoOutPut(AVSync *avsync, AVRational time_base, AVFrameQueue *frame_queue, int video_width, int video_height)
    : m_avsync (avsync), m_time_base (time_base),
      m_frame_queue (frame_queue), m_video_width (video_width), m_video_height (video_height)
{

}

int VideoOutPut::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL_Init failed" << std::endl;
        return -1;
    }

    m_win = SDL_CreateWindow("player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_video_width, m_video_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!m_win) {
        std::cout << "SDL_CreateWindow failed" << std::endl;
        return -1;
    }

    m_renderer = SDL_CreateRenderer(m_win, -1, 0);
    if (!m_renderer) {
        std::cout << "SDL_CreateRenderer failed" << std::endl;
        return -1;
    }

    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, m_video_width, m_video_height);
    if (!m_texture) {
        std::cout << "SDL_CreateTexture failed" << std::endl;
        return -1;
    }

    return 0;
}

int VideoOutPut::MainLoop()
{
    SDL_Event event;
    while (true) {
        RefreshLoopWaitEvent(&event);

        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                std::cout <<  "esc key down" << std::endl;
                return 0;
            }
            break;
        case SDL_QUIT:
            std::cout <<  "SDL_QUIT" << std::endl;
            return 0;
            break;

        default:
            break;
        }
    }
}

#define REFRESH_RATE 0.04
void VideoOutPut::RefreshLoopWaitEvent(SDL_Event *event)
{
    double remaining_time = 0.0;
    SDL_PumpEvents();
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
        if (remaining_time > 0.0)
            std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(remaining_time * 1000.0)));
        remaining_time = REFRESH_RATE;
        //尝试刷新画面
        VideoRefresh(&remaining_time);
        SDL_PumpEvents();
    }
}

void VideoOutPut::VideoRefresh(double *remaining_time)
{
    AVFrame *frame = nullptr;
    frame = m_frame_queue->Front();

    if (frame) {

        double pts = frame->pts * av_q2d(m_time_base);
        //std::cout << "video pts:" << pts << std::endl;

        double diff = pts - m_avsync->GetClock();
        if (diff > 0) {
            *remaining_time = FFMIN(diff, *remaining_time);
            return;
        }

        m_rect.x = 0;
        m_rect.y = 0;
        m_rect.w = m_video_width;
        m_rect.h = m_video_height;
        SDL_UpdateYUVTexture(m_texture, &m_rect, frame->data[0], frame->linesize[0],
                frame->data[1], frame->linesize[1],
                frame->data[2], frame->linesize[2]);

        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_rect);
        SDL_RenderPresent(m_renderer);
        frame = m_frame_queue->Pop(1);
        av_frame_free(&frame);
    }
}
