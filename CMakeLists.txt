cmake_minimum_required(VERSION 3.8)
project(xtext VERSION 0.0.4)

set(CMAKE_C_STANDARD 17)
unset(CMAKE_C_FLAGS)
set(LIBXTEXT_INSTALL_LIB True)

set(LIBXTEXT_SRC
    src/text_front.c
    src/text_obj.c
)

add_library(xtext SHARED)
target_sources(xtext PRIVATE ${LIBXTEXT_SRC})

target_compile_options(xtext PRIVATE -O2;-Wall)

target_include_directories(xtext PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(xtext PRIVATE ${CMAKE_SOURCE_DIR}/src)

set_target_properties(xtext PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(xtext PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if (${LIBXTEXT_INSTALL_LIB} AND UNIX)
    include(GNUInstallDirs)

    install(TARGETS xtext
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/xtext
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*h"
    )
    configure_file(input_configuration.pc.in input_configuration.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/input_configuration.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
