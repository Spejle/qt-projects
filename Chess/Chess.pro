QT += core gui widgets

CONFIG += c++17

TARGET = Chess
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/piecerenderer.cpp \
    src/chessgame.cpp \
    src/chessai.cpp \
    src/antigravityengine.cpp \
    src/claudehaikuengine.cpp \
    src/promotiondialog.cpp \
    src/newgamedialog.cpp \
    src/chessboardwidget.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/types.h \
    src/piecerenderer.h \
    src/chessgame.h \
    src/chessai.h \
    src/antigravityengine.h \
    src/claudehaikuengine.h \
    src/promotiondialog.h \
    src/newgamedialog.h \
    src/chessboardwidget.h \
    src/mainwindow.h

unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
