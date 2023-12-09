cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "macOS Deployment Target")
    endif()
endif()

if (APPLE)
    set (PROJECT_LANGUAGES CXX OBJCXX)
else()
    set (PROJECT_LANGUAGES CXX)
endif()


project(vectrl_proto LANGUAGES ${PROJECT_LANGUAGES})

set(wxBUILD_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libs/wxWidgets)
add_subdirectory(libs/hidapi)

set(SRC_FILES
    src/main.cpp
    src/ui_common.h
    src/main_frame.cpp
    src/main_frame.h
    src/usb.cpp
    src/usb.h
    src/actions.cpp
    src/actions.h
    src/thread_utils.cpp
    src/thread_utils.h
    src/config_loader.cpp
    src/config_loader.h
    )

set(SRC_FILES_WIN
    src/actions_win.cpp
    )

set(SRC_FILES_LINUX)

set(SRC_FILES_MACOSX
    src/actions_mac.mm
    )

if(WIN32)
    list(APPEND SRC_FILES ${SRC_FILES_WIN})
elseif(APPLE)
    list(APPEND SRC_FILES ${SRC_FILES_MACOSX})
elseif(LINUX)
    list(APPEND SRC_FILES ${SRC_FILES_LINUX})
endif()

# if(WIN32)
#     list(APPEND SRC_FILES ../sample.rc)
# elseif(APPLE)
#     list(APPEND SRC_FILES ../../src/osx/carbon/wxmac.icns)
# endif()

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} wx::net wx::core wx::base hidapi::hidapi)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
#         RESOURCE "../../src/osx/carbon/wxmac.icns"
#         MACOSX_BUNDLE_ICON_FILE wxmac.icns
        MACOSX_BUNDLE_COPYRIGHT "Copyright ***"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.developer.vectrl_proto"
        MACOSX_BUNDLE_BUNDLE_NAME "vectrl_proto"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0"
        )
endif()