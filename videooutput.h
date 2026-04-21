#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#ifdef __cplusplus

extern "C" {
#include "SDL.h"
#include "libavutil/time.h"
}
#endif
#include "avframequeue.h"
#include "avsync.h"

class VideoOutPut
{
public:
    VideoOutPut(AVSync *avsync, AVRational time_base, AVFrameQueue *frame_queue, int video_width, int video_height);

    int Init();
    int MainLoop();
    void RefreshLoopWaitEvent(SDL_Event *event);

private:
    AVSync *m_avsync = nullptr;
    AVRational m_time_base;

    void VideoRefresh(double *remaining_time);
    AVFrameQueue *m_frame_queue = nullptr;
    SDL_Event m_event;
    SDL_Rect m_rect;
    SDL_Window *m_win = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    SDL_Texture *m_texture = nullptr;

    int m_video_width;
    int m_video_height;


};

#endif // VIDEOOUTPUT_H
