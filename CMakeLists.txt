cmake_minimum_required(VERSION 3.26)
project(gta_cMecro)

option(USE_CONSOLE "use console" ON)

set(CMAKE_CXX_STANDARD 20)
set(cur_path ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(GET cur_path PARENT_PATH parent_path)
message(STATUS "PARENT_DIR: ${parent_path}")
file(READ ${parent_path}/com.txt COM_CONTENT)

set(CMAKE_C_SOURCE_FILE_EXTENSIONS lib/sqlite3.c)
set(wxWidgets_CONFIGURATION mswu)
message("current com = ${COM_CONTENT}")
if("${COM_CONTENT}" STREQUAL "sub_com")
    set(VCPKG_TOOLCHAIN C:/Users/bae/.vcpkg-clion/vcpkg_c2/scripts/buildsystems/vcpkg.cmake)
    set(wxWidgets_ROOT_DIR D:/Projects/wxWidget/3.2.4)
    set(wxWidgets_LIB_DIR D:/Projects/wxWidget/3.2.4/lib/vc14x_x64_dll)
    set(wxWidgets_INCLUDE_DIRS D:/Projects/wxWidget/3.2.4/include)
elseif("${COM_CONTENT}" STREQUAL "main_com")
    set(VCPKG_TOOLCHAIN C:/Users/creat/.vcpkg-clion/vcpkg_c2/scripts/buildsystems/vcpkg.cmake)
    set(wxWidgets_ROOT_DIR C:/Users/creat/Projects/wxWidget/3.2.4)
    set(wxWidgets_LIB_DIR C:/Users/creat/Projects/wxWidget/3.2.4/lib/vc14x_x64_dll)
    set(wxWidgets_INCLUDE_DIRS C:/Users/creat/Projects/wxWidget/3.2.4/include)
endif()
SET(wxWidgets_USE_LIBS)
FIND_PACKAGE(wxWidgets REQUIRED gl core base)
INCLUDE("${wxWidgets_USE_FILE}")

set(SOURCES
        main.cpp
        components/commander.cpp
        components/commander.h
        components/img_processor.cpp
        components/img_processor.h
        components/key_patterns.cpp
        components/key_patterns.h
        components/keyboard_hooker.cpp
        components/keyboard_hooker.h
        components/key_data.cpp
        components/key_data.h
        components/mouse_hooker.cpp
        components/mouse_hooker.h
        components/action_operator.cpp
        components/action_operator.h
        test/test000_capture.cpp
        test/test000_capture.h
        test/test001_keyboard.cpp
        test/test001_keyboard.h
        test/test002_key_pattern.cpp
        test/test002_key_pattern.h
        test/test003_arduino.cpp
        test/test003_arduino.h
        components/timer.cpp
        components/timer.h
        components/actions.h
        test/test004_sqlite3.cpp
        test/test004_sqlite3.h
        lib/sqlite3.c
        lib/sqlite3.h
        test/test006_wxWidgets.cpp
        test/test006_wxWidgets.h
        components/view/main_window.cpp
        components/view/main_window.h
        lib/CLI11.hpp
)
if(USE_CONSOLE)
    add_definitions(-DUSE_CONSOLE)
    add_executable(gta_cMecro ${SOURCES})
else()
    add_executable(gta_cMecro WIN32 ${SOURCES})
endif()
include_directories(&{PROJECT_SOURCE_DIR}/components)
include_directories(&{PROJECT_SOURCE_DIR})
target_link_libraries(gta_cMecro ${LIBS})
target_link_libraries(gta_cMecro ${wxWidgets_LIBRARIES})
target_link_libraries(gta_cMecro gdiplus)