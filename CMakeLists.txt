
cmake_minimum_required(VERSION 3.16)

project(elf VERSION 0.1)

set(libelf_SRC
    elf.c
    elf-error.c
    elf-string-data.c
)

add_library(${PROJECT_NAME} SHARED ${libelf_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

if (UNIX)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/libelf
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*h"
    )
endif()

