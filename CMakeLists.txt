cmake_minimum_required(VERSION 3.25)
project(SongGenGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Qt6_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Qt/6.5.2/mingw_64/lib/cmake/Qt6")
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR})

set(REQUIRED_LIBS Core Gui Widgets Multimedia MultimediaWidgets Network)
set(REQUIRED_LIBS_QUALIFIED winmm Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Multimedia Qt6::MultimediaWidgets)


find_package(Qt6 COMPONENTS ${REQUIRED_LIBS} REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SongGen)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SongGen/cxxmidi)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SongGen/cxxmidi/output)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SongGen/cxxmidi/guts)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SongGen/cxxmidi/player)

add_executable(SongGenGUI
        main.cpp


        App.ui App.h App.cpp

        Cell.h Cell.cpp

        ButtonEventFilter.h ButtonEventFilter.cpp
        ContextMenuHandler.h ContextMenuHandler.cpp

        CustomSlider.h
        SliderHeader.h

        ../SongGen/SongGen.h ../SongGen/SongGen.cpp
        ../SongGen/cxxmidi/guts/PlayerBase.hpp ../SongGen/cxxmidi/guts/PlayerBase.cpp
        ../SongGen/cxxmidi/output/MessageInterface.hpp ../SongGen/cxxmidi/output/MessageInterface.cpp
        ../SongGen/cxxmidi/output/Windows.hpp ../SongGen/cxxmidi/output/Windows.cpp
        ../SongGen/cxxmidi/Event.hpp ../SongGen/cxxmidi/Event.cpp
        ../SongGen/cxxmidi/File.hpp ../SongGen/cxxmidi/File.cpp
        ../SongGen/cxxmidi/Instrument.hpp ../SongGen/cxxmidi/Instrument.cpp
        ../SongGen/cxxmidi/Message.hpp ../SongGen/cxxmidi/Message.cpp
        ../SongGen/cxxmidi/Note.hpp ../SongGen/cxxmidi/Note.cpp
        ../SongGen/cxxmidi/Track.hpp ../SongGen/cxxmidi/Track.cpp
        ../SongGen/cxxmidi/player/PlayerSync.hpp ../SongGen/cxxmidi/player/PlayerSync.cpp

        )


target_link_libraries(SongGenGUI ${REQUIRED_LIBS_QUALIFIED})

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../Qt/6.5.2/mingw_64")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB ${REQUIRED_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

    endforeach (QT_LIB)
endif ()
