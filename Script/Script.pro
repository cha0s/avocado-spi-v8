TARGET = ScriptService-v8

include(../../Abstract/Script/Script.pri)

SOURCES += \
	\
	avocado-v8.cpp ObjectWrap.cpp \
	\
	v8Script.cpp v8ScriptService.cpp \
	\
	v8CoreService.cpp \
	\
	v8GraphicsService.cpp v8Font.cpp v8Image.cpp v8Window.cpp \
	\
	v8TimingService.cpp v8Counter.cpp \
	\
	v8SoundService.cpp v8Music.cpp v8Sample.cpp

HEADERS += \
	\
	avocado-v8.h ObjectWrap.h \
	\
	v8Script.h v8ScriptService.h \
	\
	v8CoreService.h \
	\
	v8GraphicsService.h v8Image.h v8Window.h \
	\
	v8TimingService.h v8Counter.h \
	\
	v8SoundService.h v8Music.h v8Sample.h

INCLUDEPATH += ../deps/v8/include
LIBS += -L../deps/v8
LIBS += -lv8-avocado -lv8_snapshot-avocado
