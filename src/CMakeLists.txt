add_subdirectory(templates)

add_executable(ppepper ppepper.cpp server.cpp inireader.cpp test.cpp)
target_link_libraries(ppepper onion onioncpp ppepper_templates)

