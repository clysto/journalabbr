cmake_minimum_required(VERSION 3.15)

project(journalabbr VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FLTK_VERSION 1.4.x-20240913-31f4a440)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lpthread -Wl,--no-whole-archive")
endif()

include(FetchContent)
FetchContent_Declare(
    fltk
    URL https://www.fltk.org/pub/fltk/snapshots/fltk-${FLTK_VERSION}.tar.gz
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(fltk)

FetchContent_Declare(rapidfuzz
    GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp.git
    GIT_TAG main
)
FetchContent_MakeAvailable(rapidfuzz)

set(APP_SOURCES
    src/main.cpp
    src/app.hpp
    src/app.cpp
    src/journal.hpp
    src/journal.cpp
    src/data.hpp
)

set(MACOSX_BUNDLE_ICON_FILE journalabbr.icns)
set(MACOSX_BUNDLE_ICON_PATH ${CMAKE_CURRENT_LIST_DIR}/assets/journalabbr.icns)
set_source_files_properties(${MACOSX_BUNDLE_ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

if(WIN32)
    add_executable(journalabbr ${APP_SOURCES} assets/chinesecheckers.rc)
elseif(APPLE)
    add_executable(journalabbr MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_PATH} ${APP_SOURCES})
endif()

target_link_libraries(journalabbr PRIVATE fltk::fltk rapidfuzz::rapidfuzz)

set_target_properties(journalabbr PROPERTIES
    MACOSX_BUNDLE on
    WIN32_EXECUTABLE on
    OUTPUT_NAME "Journal Abbr"
)
