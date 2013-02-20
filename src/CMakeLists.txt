add_subdirectory(templates)

add_executable(ppepper ppepper.cpp server.cpp inireader.cpp test.cpp utils.cpp)
target_link_libraries(ppepper onion onioncpp ppepper_templates)

install(TARGETS ppepper DESTINATION bin)
