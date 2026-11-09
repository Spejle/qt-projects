cmake_minimum_required(VERSION 3.16)

project(Chess VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

set(SOURCES
    src/main.cpp
    src/types.h
    src/piecerenderer.h
    src/piecerenderer.cpp
    src/chessgame.h
    src/chessgame.cpp
    src/chessai.h
    src/chessai.cpp
    src/antigravityengine.h
    src/antigravityengine.cpp
    src/claudehaikuengine.h
    src/claudehaikuengine.cpp
    src/promotiondialog.h
    src/promotiondialog.cpp
    src/newgamedialog.h
    src/newgamedialog.cpp
    src/chessboardwidget.h
    src/chessboardwidget.cpp
    src/mainwindow.h
    src/mainwindow.cpp
)

add_executable(Chess ${SOURCES})

target_link_libraries(Chess PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

install(TARGETS Chess
    RUNTIME DESTINATION bin
)

enable_testing()

add_executable(test_chess
    tests/test_chess.cpp
    src/piecerenderer.cpp
    src/chessgame.cpp
    src/chessai.cpp
    src/antigravityengine.cpp
    src/claudehaikuengine.cpp
    src/promotiondialog.cpp
    src/newgamedialog.cpp
    src/chessboardwidget.cpp
    src/mainwindow.cpp
)

target_link_libraries(test_chess PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

add_test(NAME ChessLogicTest COMMAND test_chess)
