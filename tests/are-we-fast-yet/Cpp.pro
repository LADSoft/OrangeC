QT       -= core
QT       -= gui

TARGET = AwfyCpp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    som/Dictionary.h \
    som/Interfaces.h \
    som/Random.h \
    som/Set.h \
    som/Vector.h \
    Benchmark.h \
    Bounce.h \
    CD.h \
    DeltaBlue.h \
    Havlak.h \
    Json.h \
    List.h \
    Mandelbrot.h \
    NBody.h \
    Object.h \
    Permute.h \
    Queens.h \
    RedBlackTree.h \
    Richards.h \
    Run.h \
    Sieve.h \
    Storage.h \
    Towers.h

SOURCES += \
    som/Random.cpp \
    Bounce.cpp \
    CD.cpp \
    DeltaBlue.cpp \
    Havlak.cpp \
    Json.cpp \
    List.cpp \
    main.cpp \
    Mandelbrot.cpp \
    NBody.cpp \
    Permute.cpp \
    Queens.cpp \
    Richards.cpp \
    Run.cpp \
    Sieve.cpp \
    Storage.cpp \
    Towers.cpp


