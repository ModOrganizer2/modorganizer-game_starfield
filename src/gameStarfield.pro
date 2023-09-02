#-------------------------------------------------
#
# Project created by QtCreator 2014-11-15T15:36:33
#
#-------------------------------------------------


TARGET = gameStarfield
TEMPLATE = lib

CONFIG += plugins
CONFIG += dll

DEFINES += GAMESTARFIELD_LIBRARY

SOURCES += gamestarfield.cpp \
    starfieldbsainvalidation.cpp \
    starfieldscriptextender.cpp \
    starfielddataarchives.cpp \
    starfieldsavegame.cpp \
    starfieldsavegameinfo.cpp

HEADERS += gamestarfield.h \
    starfieldbsainvalidation.h \
    starfieldscriptextender.h \
    starfielddataarchives.h \
    starfieldsavegame.h \
    starfieldsavegameinfo.h

CONFIG(debug, debug|release) {
  LIBS += -L"$${OUT_PWD}/../gameGamebryo/debug"
  PRE_TARGETDEPS += \
    $$OUT_PWD/../gameGamebryo/debug/gameGamebryo.lib
} else {
  LIBS += -L"$${OUT_PWD}/../gameGamebryo/release"
  PRE_TARGETDEPS += \
    $$OUT_PWD/../gameGamebryo/release/gameGamebryo.lib
}

include(../plugin_template.pri)

INCLUDEPATH += "$${BOOSTPATH}" "$${PWD}/../gamefeatures" "$${PWD}/../gamegamebryo"

LIBS += -ladvapi32 -lole32 -lgameGamebryo

OTHER_FILES += \
    gamestarfield.json\
    SConscript \
    CMakeLists.txt

