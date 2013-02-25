add_subdirectory(templates)

add_executable(garlic garlic.cpp server.cpp inireader.cpp test.cpp utils.cpp)
target_link_libraries(garlic onion onioncpp garlic_templates)

install(TARGETS garlic DESTINATION bin)
