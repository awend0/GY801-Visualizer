QT       += core gui 3dcore 3drender 3dinput 3dlogic 3dextras 3danimation opengl serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


SOURCES += \
    console.cpp \
    main.cpp \
    gy801.cpp

HEADERS += \
    console.h \
    gy801.h

FORMS += \
    gy801.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
