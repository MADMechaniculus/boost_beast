TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += qt

SOURCES += \
        abstractapplication.cpp \
        eventloopapplication.cpp \
        main.cpp \
        messagehandler.cpp \
        requesthandler.cpp

DISTFILES += \
        .gitignore

HEADERS += \
        abstractapplication.h \
        defines.h \
        eventloopapplication.h \
        messagehandler.h \
        requesthandler.h

win32 {
INCLUDEPATH += "C:\\include\\boost_1_76_0"

LIBS += -L"C:\\libs\\libboost_1.76.0" \
        -L"C:\\libs\\boost_1.76.0" \
        -lboost_system-mgw7-mt-x32-1_76 \
        -lboost_thread-mgw7-mt-x32-1_76 \
        -lboost_filesystem-mgw7-mt-x32-1_76 \
        -lboost_regex-mgw7-mt-x32-1_76 \
        -lboost_program_options-mgw7-mt-x32-1_76 \
        -lboost_atomic-mgw7-mt-x32-1_76 \
        -lpthread \
        -lws2_32 \
        -lmswsock
}

unix {
INCLUDEPATH += "/usr/include/libboost_1.76.0"

LIBS += -L"/usr/lib/libboost_1.76.0" \
        -lboost_system \
        -lboost_thread \
        -lboost_filesystem \
        -lboost_regex \
        -lboost_program_options \
        -lpthread
}
