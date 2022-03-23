TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += qt

SOURCES += \
        abstractapplication.cpp \
        abstractgetproc.cpp \
        abstractpostproc.cpp \
        eventloopapplication.cpp \
        main.cpp \
        requesthandler.cpp \
        keysightcommand/keysightcommand.cpp \
        usergetproc.cpp \
        userpostproc.cpp

DISTFILES += \
        .gitignore \
        README.md

HEADERS += \
        abstractapplication.h \
        abstractgetproc.h \
        abstractpostproc.h \
        defines.h \
        eventloopapplication.h \
        proctypes.h \
        requesthandler.h \
        keysightcommand/keysightcommand.h \
        usergetproc.h \
        userpostproc.h

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
        -lboost_json-mgw7-mt-x32-1_76 \
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
