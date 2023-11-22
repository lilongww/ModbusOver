function(export_install_target target_name includedir libdir bindir)
    install(TARGETS ${target_name}
        EXPORT ${target_name}
        ARCHIVE DESTINATION ${libdir}
        RUNTIME DESTINATION ${bindir}/${CMAKE_BUILD_TYPE}
        PUBLIC_HEADER DESTINATION ${includedir}/${target_name})

    install(EXPORT ${target_name} FILE "${target_name}Targets.cmake" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/cmake")
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cmake" DESTINATION "${libdir}" FILES_MATCHING PATTERN "*.cmake")
endfunction()