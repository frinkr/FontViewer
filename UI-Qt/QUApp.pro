QT += widgets core network
requires(qtConfig(filedialog))

macx {
    CONFIG += cocoa
    OBJECTIVE_SOURCES += \
        MacMain.mm \
        MacDocumentWindowManager.mm \
        MacApplicationDelegate.mm
    HEADERS += MacApplicationDelegate.h
    QMAKE_INFO_PLIST = MacInfo.plist
    LIBS += -framework Cocoa
} else {
    SOURCES += QUMain.cpp
}

SOURCES += \
    QUDocumentWindow.cpp \
    QUDocumentWindowManager.cpp \
    QUSingleApplication.cpp \
    QUApplication.cpp \

HEADERS += \
    QUDocumentWindow.h \
    QUDocumentWindowManager.h \
    QUSingleApplication.h \
    QUSingleApplication_p.h \
    QUApplication.h \
    QUMain.h

win32 {
    msvc:LIBS += Advapi32.lib
    gcc:LIBS += -lAdvapi32
}

RESOURCES     = QUApp.qrc
