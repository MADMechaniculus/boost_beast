TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        messagehandler.cpp \
        requesthandler.cpp

INCLUDEPATH += "C:\\include\\boost_1_76_0"

LIBS += -L"C:\\libs\\boost_1.76.0" \
        -lboost_system-mgw7-mt-x32-1_76 \
        -lboost_thread-mgw7-mt-x32-1_76 \
        -lboost_filesystem-mgw7-mt-x32-1_76 \
        -lboost_regex-mgw7-mt-x32-1_76 \
        -lpthread \
        -lws2_32 \
        -lmswsock

DISTFILES += .gitignore

HEADERS += \
    messagehandler.h \
    requesthandler.h
