# odio-sacd

add_executable ("odio-sacd" main.c)
target_compile_definitions ("odio-sacd" PUBLIC APPVERSION="${CMAKE_PROJECT_VERSION}")
target_link_libraries ("odio-sacd" ${DEPS_LIBRARIES})
install (TARGETS "odio-sacd" RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}")
