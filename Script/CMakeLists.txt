cmake_minimum_required(VERSION 2.8)

add_definitions(-DAVOCADO)

# v8 maintainers, I am disappoint.
string(REPLACE "-ansi -Wall -Werror" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

add_library(ScriptService-v8 SHARED
	
	implementSpi.cpp
	
	avocado-v8.cpp
	ObjectWrap.cpp
	
	v8ScriptService.cpp
	v8Script.cpp
	
	v8CoreService.cpp
	
	v8GraphicsService.cpp
	v8Canvas.cpp
	v8Font.cpp
	v8Image.cpp
	v8Sprite.cpp
	v8Window.cpp
	
	v8TimingService.cpp
	
	v8SoundService.cpp
	v8Music.cpp
	v8Sample.cpp
	
)

target_link_libraries(ScriptService-v8

	avocado-v8_base.ia32
	avocado-v8_snapshot
	
	boost_filesystem
	boost_regex
	boost_system
	
	dl

)

if(WIN32)

	target_link_libraries(ScriptService-v8
	
		shell32
		ws2_32
		winmm

	)
	
endif()

avocado_build_spii_name(ScriptService-v8)
