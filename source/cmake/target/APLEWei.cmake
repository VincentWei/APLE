if (NOT TARGET APLEWei::APLEWei)
    if (NOT INTERNAL_BUILD)
        message(FATAL_ERROR "APLEWei::APLEWei target not found")
    endif ()

    # This should be moved to an if block if the Apple Mac/iOS build moves completely to CMake
    # Just assuming Windows for the moment
    add_library(APLEWei::APLEWei STATIC IMPORTED)
    set_target_properties(APLEWei::APLEWei PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIBRARIES_LINK_DIR}/APLEWei${DEBUG_SUFFIX}.lib
    )
    set(APLEWei_PRIVATE_FRAMEWORK_HEADERS_DIR "${CMAKE_BINARY_DIR}/../include/private")
    target_include_directories(APLEWei::APLEWei INTERFACE
        ${APLEWei_PRIVATE_FRAMEWORK_HEADERS_DIR}
    )
endif ()
