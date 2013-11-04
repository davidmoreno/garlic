add_custom_command(
   OUTPUT base_html.c
   COMMAND ${OTEMPLATE} ${CMAKE_CURRENT_SOURCE_DIR}/base.html
                 ${CMAKE_CURRENT_BINARY_DIR}/base_html.c
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/base.html
   )

add_custom_command(
   OUTPUT login_html.c
   COMMAND ${OTEMPLATE} ${CMAKE_CURRENT_SOURCE_DIR}/login.html
                 ${CMAKE_CURRENT_BINARY_DIR}/login_html.c
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/login.html
   )

add_custom_command(
   OUTPUT index_html.c
   COMMAND ${OTEMPLATE} ${CMAKE_CURRENT_SOURCE_DIR}/index.html
                 ${CMAKE_CURRENT_BINARY_DIR}/index_html.c
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/index.html
   )


add_library(garlic_templates STATIC base_html.c login_html.c index_html.c)
