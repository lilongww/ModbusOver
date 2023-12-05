function(modbusover_add_test name)
    set(multiopts SOURCES PRIVATE_LIBRARIES PUBLIC_LIBRARIES)
    cmake_parse_arguments(arg "${flags}" "${options}" "${multiopts}" ${ARGN})
    include_directories(${PROJECT_SOURCE_DIR}/src/3rd ${PROJECT_SOURCE_DIR}/src)
    add_executable(${name} ${arg_SOURCES})
    target_link_libraries(${name} PUBLIC ${arg_PUBLIC_LIBRARIES} PRIVATE "${arg_PRIVATE_LIBRARIES}")
    set_target_properties(${name} PROPERTIES "FOLDER" "Tests")
    install(TARGETS ${name} RUNTIME DESTINATION ${INSTALL_BINDIR}/${CMAKE_BUILD_TYPE})
    add_test(NAME "${name}" COMMAND ${name} WORKING_DIRECTORY ${INSTALL_BINDIR}/${CMAKE_BUILD_TYPE})
endfunction(modbusover_add_test)