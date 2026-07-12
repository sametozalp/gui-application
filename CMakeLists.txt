cmake_minimum_required(VERSION 3.16)

project(gui-application VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick QuickControls2)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick QuickControls2)

set(PROJECT_SOURCES
    main.cpp
    countermanager.h
    countermanager.cpp
    counterpresenter.h
    counterpresenter.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_standard_project_setup(REQUIRES 6.5)

    qt_add_executable(gui-application
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_add_qml_module(gui-application
        URI GuiApplication
        VERSION 1.0
        QML_FILES
            Main.qml
    )
else()
    find_package(Qt5 REQUIRED COMPONENTS Quick)
    add_executable(gui-application ${PROJECT_SOURCES})
    qt5_add_resources(gui-application "qml"
        PREFIX "/"
        FILES Main.qml
    )
endif()

target_link_libraries(gui-application PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gui-application)
endif()
set_target_properties(gui-application PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS gui-application
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gui-application)
endif()
