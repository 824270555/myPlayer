TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    avsync.cpp \
        main.cpp \
    avpacketqueue.cpp \
    avframequeue.cpp \
    demuxthread.cpp \
    decodethread.cpp \
    videooutput.cpp \
    audiooutput.cpp

win32 {
INCLUDEPATH += $$PWD/SDL2-2.0.10/include

LIBS += $$PWD/SDL2-2.0.10/lib/x86/SDL2.lib

LIBS += $$PWD/lib/avcodec.lib \
       $$PWD/lib/avfilter.lib \
        $$PWD/lib/avformat.lib \
        $$PWD/lib/avutil.lib \
        $$PWD/lib/postproc.lib \
        $$PWD/lib/swresample.lib \
        $$PWD/lib/swscale.lib


INCLUDEPATH += $$PWD/include
}

HEADERS += \
    avsync.h \
    thread.h \
    queue.h \
    avpacketqueue.h \
    avframequeue.h \
    demuxthread.h \
    decodethread.h \
    videooutput.h \
    audiooutput.h
