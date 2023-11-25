cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10 CACHE STRING "macOS Deployment Target")
    endif()
endif()

project(vectrl_proto)

set(wxBUILD_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libs/wxWidgets)
add_subdirectory(libs/hidapi)

set(SRC_FILES
    src/main.cpp
    src/serial.cpp
    src/serial.h
    src/actions.cpp
    src/actions.h
    )

# if(WIN32)
#     list(APPEND SRC_FILES ../sample.rc)
# elseif(APPLE)
#     list(APPEND SRC_FILES ../../src/osx/carbon/wxmac.icns)
# endif()

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} wx::net wx::core wx::base hidapi::hidapi)

# if(APPLE)
#     set_target_properties(${PROJECT_NAME} PROPERTIES
#         RESOURCE "../../src/osx/carbon/wxmac.icns"
#         MACOSX_BUNDLE_ICON_FILE wxmac.icns
#         MACOSX_BUNDLE_COPYRIGHT "Copyright wxWidgets"
#         MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxwidgets.minimal"
#         )
# endif()