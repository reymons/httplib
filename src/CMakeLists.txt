cmake_minimum_required(VERSION 3.2)
project(httplib VERSION 1.0 LANGUAGES C)

add_library(server STATIC lib/server.c)
add_library(event STATIC lib/event.c)
add_library(logger STATIC lib/logger.c)

add_executable(app example_server.c)

target_link_libraries(event PUBLIC logger)
target_link_libraries(server PUBLIC event)
target_link_libraries(app PRIVATE server)
