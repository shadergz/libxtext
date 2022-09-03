cmake_minimum_required(VERSION 3.8)
project(bin VERSION 0.0.3)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Werror -Wall -O2")

set(LIBBIN_INSTALL_LIB TRUE)

set(LIBBIN_SRC
        src/binfront.c
        src/elfobject.c
        src/symbol.c src/object.h)

add_library(${PROJECT_NAME} SHARED ${LIBBIN_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE include)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if(${LIBBIN_INSTALL_LIB})
if (UNIX)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/bin
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*h"
    )
    configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
endif()
