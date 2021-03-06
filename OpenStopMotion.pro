TEMPLATE = app
TARGET = OpenStopMotion
QT += core \
    gui \
    opengl
CONFIG += debug_and_release
CONFIG(debug, debug|release) { 
    win32:TARGET = OpenStopMotion
    unix:TARGET = debug/OpenStopMotion
}
else { 
    win32:TARGET = OpenStopMotion
    unix:TARGET = release/OpenStopMotion
}
HEADERS += include/osm.h \ 
	src/FrameBuffer.h \
	src/arduino.h \
	gui/ledslider.h \
	gui/ledcontrol.h \
	gui/framepaint.h \
	gui/ppl7framepainter.h \
    gui/stopmocap.h \
    gui/selectscene.h \
    src/savethread.h \
    gui/myslider.h \
    gui/ppl7imageviewer.h \
    src/device.h \
    include/config.h \
    gui/arduinosketch.h \
    src/bluebox.h \
    gui/release.h \
    gui/motioncontrol.h \
    src/picontrol.h
    
SOURCES += src/FrameBuffer.cpp \
	src/arduino.cpp \
	gui/ledslider.cpp \
	gui/ledcontrol.cpp \
	gui/framepaint.cpp \
	gui/ppl7framepainter.cpp \
    gui/stopmocap.cpp \
    gui/selectscene.cpp \
    src/savethread.cpp \
    gui/myslider.cpp \
    gui/ppl7imageviewer.cpp \
    src/device_v4l2.cpp \
    src/device_win32.cpp \
    src/main.cpp \
    src/config.cpp \
    src/bluebox.cpp \
    gui/release.cpp \
    gui/motioncontrol.cpp \
    src/picontrol.cpp
    
FORMS += gui/stopmocap.ui \
	gui/ledslider.ui \
	gui/ledcontrol.ui \
	gui/framepaint.ui \
    gui/selectscene.ui \
    gui/myslider.ui \
    gui/arduinosketch.ui \
    gui/release.ui \
    gui/motioncontrol.ui
    
RESOURCES += resources.qrc
RC_FILE = resource.rc
INCLUDEPATH += gui
INCLUDEPATH += src
unix:INCLUDEPATH += ../tmp/include
unix:INCLUDEPATH += ./include
unix:INCLUDEPATH += /usr/local/include
win32:INCLUDEPATH += C:/mingw/usr/local/include
win32:QMAKE_LIBDIR += C:/mingw/usr/local/lib
win32:QMAKESPEC = C:/Qt/4.7.4/mkspecs/win32-g++
CONFIG(debug, debug|release) { 
    # Debug
    unix:LIBS += `ppl7-config \
        --libs \
        debug`
    win32:LIBS += -lppl7-debug \
        -lpcre \
        -liconv \
        -lmp3lame \
        -lz \
        -lbz2 \
        -lcurl \
        -lidn \
        -lssl \
        -lmcrypt \
        -lcrypto \
        -lws2_32 \
        -ldnsapi \
        -lz \
        -lbz2 \
        -lvfw32 \
        -lgdi32
}
else { 
    # Release
    unix:LIBS += `ppl7-config \
        --libs \
        release`
    win32:LIBS += -lppl7 \
        -lpcre \
        -liconv \
        -lmp3lame \
        -lz \
        -lbz2 \
        -lcurl \
        -lidn \
        -lssl \
        -lmcrypt \
        -lcrypto \
        -lws2_32 \
        -ldnsapi \
        -lz \
        -lbz2 \
        -lvfw32 \
        -lgdi32
}
CODECFORSRC = UTF-8
CODECFORTR = UTF-8
