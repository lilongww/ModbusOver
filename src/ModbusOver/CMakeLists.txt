set(TARGET_NAME ModbusOver)

add_compile_definitions(MODBUSOVER_LIB)
if(BUILD_MODBUSOVER_STATIC)
    add_compile_definitions(BUILD_MODBUSOVER_STATIC)
    set(MODBUSOVER_OPT)
else()
    set(MODBUSOVER_OPT SHARED)
endif()

find_package(Boost COMPONENTS system REQUIRED)

file(GLOB HEADERS "*.h")
file(GLOB PRIVATES "Private/*.h" "Private/*.cpp")
file(GLOB SOURCES "*.cpp")
file(GLOB DOCS "Docs/*.qdoc")
file(GLOB RESOURCES "*.rc")

if(MSVC)
    set(MODBUSOVER_VERSIONINFO_RC ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
    configure_file("${PROJECT_SOURCE_DIR}/cmake/${TARGET_NAME}.rc.in" "${MODBUSOVER_VERSIONINFO_RC}")
    list(APPEND RESOURCES ${MODBUSOVER_VERSIONINFO_RC}) 
endif(MSVC)

set(MODBUSOVER_VERSION_H ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
configure_file("${PROJECT_SOURCE_DIR}/cmake/Version.h.in" "${MODBUSOVER_VERSION_H}")
list(APPEND HEADERS ${MODBUSOVER_VERSION_H}) 

source_group("Headers" FILES ${HEADERS})
source_group("Sources" FILES ${SOURCES})
source_group("Privates" FILES ${PRIVATES})
source_group("Docs" FILES ${DOCS})
source_group("Resources" FILES ${RESOURCES})

add_library(${TARGET_NAME} ${MODBUSOVER_OPT}
    ${HEADERS}
    ${SOURCES}
    ${PRIVATES}
    ${DOCS}
    ${RESOURCES}
    )

if(NOT BUILD_MODBUSOVER_STATIC)
    target_link_libraries(${TARGET_NAME} PUBLIC Setupapi PRIVATE Boost::system)
endif()
target_compile_options(${TARGET_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")
export_install_target(${TARGET_NAME} ${INSTALL_INCLUDEDIR} ${INSTALL_LIBRARYDIR} ${INSTALL_BINDIR})
