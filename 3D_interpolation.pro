#QMAKE_CXXFLAGS += -fsanitize=address
#QMAKE_LFLAGS += -fsanitize=address
#CONFIG += debug
HEADERS       = scene.h interpolation.h
SOURCES       = main.cpp scene.cpp interpolation.cpp
QT += widgets
QT += opengl
