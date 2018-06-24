# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}
# ensure one "debug" or "release" in CONFIG so they can be used as
# conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release force_debug_info
}

QT += core gui widgets opengl network
!android{
    QT += webkit webkitwidgets
}
android{
    QT += androidextras
}
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

win32-msvc* {
    QMAKE_CXXFLAGS += /wd4251
    DEFINES += _VARIADIC_MAX=6
}

unix:!macx{
    QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-local-typedefs
}

unix:macx{
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
}

CONFIG += c++11 -opengl desktop -no-angle

TEMPLATE = app
TARGET = janusvr_websurface

CONFIG(debug) {
    DESTDIR = bin/debug
}
CONFIG(release) {
    DESTDIR = bin/release
}


SOURCES += \
    offscreenwebsurface.cpp \
    abstractwebview.cpp \
    webview.cpp \
    cookiejar.cpp \
    childprocesscookiejar.cpp \
    mathutil.cpp \
    janusvr_websurface_main.cpp

HEADERS += offscreenwebsurface.h \
    abstractwebview.h \
    webview.h \
    cookiejar.h \
    ChildProcessCookieJar.h \
    mathutil.h

#Android
android:SOURCES -= webview.cpp
android:HEADERS -= webview.h
android:SOURCES += androidwebview.cpp \
    jniutil.cpp
android:HEADERS += androidwebview.h \
    jniutil.h

# gli
INCLUDEPATH += "./resources/gli"
INCLUDEPATH += "./resources/gli/external/glm"

# Generic Windows libs
win32:LIBS += -lopengl32 -lglu32 -ladvapi32 -lwinmm

unix:!macx:!android:LIBS += -lX11 -lXrandr -lXinerama -ludev -lGLU -lrt -ldl -lopenal -lz
unix:!macx:!android:{
   QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"
}

unix:macx:LIBS += -lz -framework IOKit -framework CoreFoundation
unix:macx:{
   QMAKE_LFLAGS += -Wl,-rpath,"@loader_path/../Frameworks"
}

android:LIBS += -ljnigraphics -landroid -lz -lOpenSLES

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    INCLUDEPATH += "./resources/androidAPI21/arch-arm/usr/include"
}

contains(ANDROID_TARGET_ARCH,armeabi) {
    INCLUDEPATH += "./resources/androidAPI21/arch-arm/usr/include"
}

contains(ANDROID_TARGET_ARCH,x86) {
    INCLUDEPATH += "./resources/androidAPI21/arch-x86/usr/include"
}

# OpenSSL
INCLUDEPATH +="./resources/openssl_win64/include"
win32:LIBS += -L"$$PWD/resources/openssl_win64/libs"
win32:LIBS += -llibeay32 -lssleay32
OPENSSL_LIBS ='-L"$$PWD/resources/openssl_win64/libs" -llibeay32 -lssleay32'
CONFIG += openssl-linked

RESOURCES +=

DISTFILES +=

android:DISTFILES += \
    android/src/org/janus/JanusActivity.java\
    android/src/org/janus/ButtonView.java\
    android/src/org/janus/JoystickView.java

# OpenGL
DEFINES += OPENGL
