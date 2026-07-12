cmake_minimum_required(VERSION 3.16)

project(gui-application VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick QuickControls2 Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick QuickControls2 Network)

# --- Host UI ---
set(HOST_SOURCES
    main.cpp
    hostbridge.h
    hostbridge.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_standard_project_setup(REQUIRES 6.5)

    qt_add_executable(gui-application
        MANUAL_FINALIZATION
        ${HOST_SOURCES}
    )

    qt_add_qml_module(gui-application
        URI GuiApplication
        VERSION 1.0
        QML_FILES
            Main.qml
    )
else()
    add_executable(gui-application ${HOST_SOURCES})
    qt5_add_resources(gui-application "qml"
        PREFIX "/"
        FILES Main.qml
    )
endif()

target_link_libraries(gui-application PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Network
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gui-application)
endif()

set(COMMON_RUNTIME_DIR
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
)

set_target_properties(gui-application PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    ${COMMON_RUNTIME_DIR}
)

# --- Sub-app A ---
add_executable(subapp-a
    subapp-a/main.cpp
)
target_link_libraries(subapp-a PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)
set_target_properties(subapp-a PROPERTIES
    WIN32_EXECUTABLE FALSE
    ${COMMON_RUNTIME_DIR}
)

# --- Sub-app B ---
add_executable(subapp-b
    subapp-b/main.cpp
)
target_link_libraries(subapp-b PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)
set_target_properties(subapp-b PROPERTIES
    WIN32_EXECUTABLE FALSE
    ${COMMON_RUNTIME_DIR}
)

include(GNUInstallDirs)
install(TARGETS gui-application subapp-a subapp-b
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gui-application)
endif()
