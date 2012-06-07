TEMPLATE = app
TARGET = StopMoCap
QT += core \
    gui
CONFIG += debug_and_release
CONFIG(debug, debug|release) { 
    win32:TARGET = StopMoCap
    unix:TARGET = debug/StopMoCap
}
else { 
    win32:TARGET = StopMoCap
    unix:TARGET = release/StopMoCap
}
HEADERS += myslider.h \
    config.h \
    stopmocap.h \
    device.h
SOURCES += myslider.cpp \
    device_win32.cpp \
    device_v4l2.cpp \
    config.cpp \
    renderthread.cpp \
    main.cpp \
    stopmocap.cpp
FORMS += myslider.ui \
    stopmocap.ui
RESOURCES += resources.qrc
RC_FILE = resource.rc
INCLUDEPATH += include
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
TRANSLATIONS = stopmocap_de.ts \
    stopmocap_en.ts
