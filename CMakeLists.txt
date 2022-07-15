
cmake_minimum_required(VERSION 3.16)

project(libbin VERSION 0.0.2)

set(LIBBIN_SRC
    bingeneric.c
)

add_library(${PROJECT_NAME} SHARED ${LIBBIN_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if (UNIX)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/bin
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*h"
    )
endif()

