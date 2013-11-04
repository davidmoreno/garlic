add_subdirectory(templates)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
   OUTPUT static.c
   COMMAND ${OPACK} ${CMAKE_CURRENT_SOURCE_DIR}/static
                 -o ${CMAKE_CURRENT_BINARY_DIR}/static.c
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/static/garlic.css ${CMAKE_CURRENT_SOURCE_DIR}/static/garlic.js
   )


add_executable(garlic garlic.cpp server.cpp inireader.cpp test.cpp utils.cpp static.c)
target_link_libraries(garlic onion onioncpp garlic_templates)

install(TARGETS garlic DESTINATION bin)
