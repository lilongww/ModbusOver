set(TARGET_NAME ModbusCpp)

include(${PROJECT_SOURCE_DIR}/cmake/ExportHelper.cmake)

add_compile_definitions(MODBUSCPP_LIB)
if(BUILD_MODBUSCPP_STATIC)
    add_compile_definitions(BUILD_MODBUSCPP_STATIC)
    set(MODBUSCPP_OPT)
else()
    set(MODBUSCPP_OPT SHARED)
endif()

find_package(Boost COMPONENTS system REQUIRED)

file(GLOB HEADERS "*.h")
file(GLOB PRIVATES "Private/*.h" "Private/*.cpp")
file(GLOB SOURCES "*.cpp")
file(GLOB DOCS "Docs/*.qdoc")
file(GLOB RESOURCES "*.rc")

if(MSVC)
    set(MODBUSCPP_VERSIONINFO_RC ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
    configure_file("${PROJECT_SOURCE_DIR}/cmake/${TARGET_NAME}.rc.in" "${MODBUSCPP_VERSIONINFO_RC}")
    list(APPEND RESOURCES ${MODBUSCPP_VERSIONINFO_RC}) 
endif(MSVC)

set(MODBUSCPP_VERSION_H ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
configure_file("${PROJECT_SOURCE_DIR}/cmake/Version.h.in" "${MODBUSCPP_VERSION_H}")
list(APPEND HEADERS ${MODBUSCPP_VERSION_H}) 

source_group("Headers" FILES ${HEADERS})
source_group("Sources" FILES ${SOURCES})
source_group("Privates" FILES ${PRIVATES})
source_group("Docs" FILES ${DOCS})
source_group("Resources" FILES ${RESOURCES})

add_library(${TARGET_NAME} ${MODBUSCPP_OPT}
    ${HEADERS}
    ${SOURCES}
    ${PRIVATES}
    ${DOCS}
    ${RESOURCES}
    )

if(NOT BUILD_MODBUSCPP_STATIC)
    target_link_libraries(${TARGET_NAME} PUBLIC Setupapi PRIVATE Boost::system)
endif()
set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

export_install_target(${TARGET_NAME} ${INSTALL_INCLUDEDIR} ${INSTALL_LIBRARYDIR} ${INSTALL_BINDIR})
