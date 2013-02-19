add_subdirectory(templates)

add_executable(ppepper ppepper.cpp)
target_link_libraries(ppepper onion onioncpp ppepper_templates)

