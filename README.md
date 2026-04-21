# FFmpeg + SDL2 Media Player
Lightweight Cross-Platform Media Player | 轻量级跨平台音视频播放器

---

## 🌟 About This Project
This project is a **lightweight, modular media player** written in C++, built on top of FFmpeg and SDL2.  
It implements the full audio/video playback pipeline:
`Demux → Decode → Synchronize → Render`

The design focuses on clarity and readability, making it an excellent learning resource for anyone who wants to understand the fundamentals of multimedia processing and playback.

---

## ✨ Core Features
- **Multi-threaded Architecture**
  Demuxing, audio decoding, and video decoding run in separate threads to avoid blocking and improve responsiveness.
- **Precise Audio/Video Synchronization**
  A custom clock mechanism based on PTS (Presentation Time Stamp) ensures audio and video stay perfectly in sync.
- **Audio Resampling**
  Uses `libswresample` to dynamically convert audio formats, sample rates, and channel layouts for compatibility with SDL2.
- **YUV Video Rendering**
  Efficiently renders raw YUV frames using SDL2 textures, supporting common pixel formats like YUV420P.
- **Thread-safe Data Queues**
  A generic, reusable queue template with mutexes and condition variables manages safe communication between threads.
- **Cross-platform Support**
  Works on Windows, Linux, and macOS, thanks to the portability of FFmpeg and SDL2.
- **Clean, Extensible Codebase**
  Modular design with well-defined components makes it easy to add new features like controls, filters, or format support.

---

## 🧱 Project Architecture
The player is divided into several independent, loosely coupled components:

| Module         | Description                                                                 |
|----------------|-----------------------------------------------------------------------------|
| `DemuxThread`  | Parses the input media file and separates audio and video packets.          |
| `DecodeThread` | Decodes compressed packets into raw audio/video frames.                    |
| `AudioOutput`  | Handles audio playback, including resampling and clock synchronization.      |
| `VideoOutput`  | Renders raw YUV video frames to the screen using SDL2.                      |
| `AVSync`       | Implements the audio/video synchronization logic based on the audio clock.   |
| `Queue<T>`     | A thread-safe generic queue used for passing packets and frames between threads. |

---

## 📖 How It Works
1.  **Demuxing**
    The `DemuxThread` reads the media file and extracts audio and video packets, pushing them into separate queues.
2.  **Decoding**
    Separate audio and video `DecodeThread`s take packets from the queues and decode them into raw frames.
3.  **Audio Playback**
    The `AudioOutput` module takes decoded frames, resamples them if necessary, and feeds the samples to the SDL2 audio device. It also updates the master clock based on the audio PTS.
4.  **Video Rendering**
    The `VideoOutput` module takes decoded frames, compares their PTS to the master audio clock, and renders them at the correct time using SDL2 textures.
5.  **Synchronization**
    The `AVSync` class provides a shared clock, ensuring the video frames are displayed in time with the audio.

---

## 📌 Usage Notes
- Press `ESC` or close the player window to exit playback.
- Supports most common media formats, including MP4, FLV, MKV, AVI, and MOV.
- This project is intended for educational purposes only.

---

## 📄 License
This project is released for learning purposes. Please ensure compliance with the licenses of FFmpeg and SDL2 if you use or modify this code.

---

---

# FFmpeg + SDL2 音视频播放器
轻量级跨平台音视频播放器

---

## 🌟 项目介绍
本项目是一个基于 C++ 开发的**轻量级、模块化音视频播放器**，核心依赖 FFmpeg 和 SDL2 实现。  
它完整实现了音视频播放的核心流程：
`解封装 → 解码 → 同步 → 渲染`

项目代码结构清晰，逻辑易懂，是学习音视频底层原理、多媒体处理流程的绝佳实践项目。

---

## ✨ 核心特性
- **多线程架构**
  解封装、音频解码、视频解码在独立线程中运行，避免阻塞，提升播放流畅度。
- **精准音视频同步**
  基于 PTS（显示时间戳）实现自定义时钟，确保音视频播放完全同步。
- **音频重采样支持**
  利用 `libswresample` 动态转换音频格式、采样率和声道布局，适配 SDL2 音频设备。
- **YUV 视频渲染**
  通过 SDL2 纹理高效渲染 YUV 原始视频帧，支持 YUV420P 等常见格式。
- **线程安全数据队列**
  基于互斥锁和条件变量实现的通用线程安全队列，用于线程间安全传递数据包和帧数据。
- **跨平台兼容**
  依托 FFmpeg 和 SDL2 的跨平台特性，支持 Windows、Linux、macOS 系统。
- **模块化易扩展**
  各组件职责清晰、耦合度低，便于添加播放控制、滤镜、新格式支持等功能。

---

## 🧱 项目架构
播放器由多个独立、低耦合的模块组成：

| 模块           | 功能说明                                                                 |
|----------------|--------------------------------------------------------------------------|
| `DemuxThread`  | 读取媒体文件，分离音频和视频数据包。                                      |
| `DecodeThread` | 将压缩的数据包解码为原始音频/视频帧。                                    |
| `AudioOutput`  | 负责音频播放，包括重采样和同步时钟更新。                                  |
| `VideoOutput`  | 使用 SDL2 将 YUV 原始视频帧渲染到屏幕。                                   |
| `AVSync`       | 基于音频时钟实现音视频同步逻辑。                                         |
| `Queue<T>`     | 通用线程安全队列模板，用于在线程间传递数据包和帧数据。                     |

---

## 📖 工作流程
1.  **解封装**
    `DemuxThread` 读取媒体文件，提取音频和视频数据包，并分别推送到对应的队列中。
2.  **解码**
    独立的音频/视频 `DecodeThread` 从队列中取出数据包，解码为原始音视频帧。
3.  **音频播放**
    `AudioOutput` 模块取出解码后的音频帧，必要时进行重采样，然后将样本送入 SDL2 音频设备播放。同时，根据音频 PTS 更新主时钟。
4.  **视频渲染**
    `VideoOutput` 模块取出解码后的视频帧，将其 PTS 与主音频时钟进行比较，然后在正确的时间通过 SDL2 纹理渲染到屏幕。
5.  **同步控制**
    `AVSync` 类提供统一的同步时钟，确保视频帧的显示时间与音频播放进度一致。

---

## 📌 使用说明
- 按 `ESC` 键或关闭播放器窗口即可退出播放。
- 支持 MP4、FLV、MKV、AVI、MOV 等主流媒体格式。
- 本项目仅用于学习与技术交流。

---

## 📄 许可证
本项目仅供学习使用。在使用或修改代码时，请确保遵守 FFmpeg 和 SDL2 的开源许可证。
