#QMAKE_CXXFLAGS += -fsanitize=address
#QMAKE_LFLAGS += -fsanitize=address
#CONFIG += debug
HEADERS       = scene3D.h interpolation.h
SOURCES       = main.cpp scene3D.cpp interpolation.cpp
QT += widgets
QT += opengl
