cmake_minimum_required(VERSION 3.4.1)

set(source

)

add_library(
        fileaccessor
        STATIC
        ${source}
      )

target_include_directories(
    fileaccessor
    PRIVATE
    ./
    ./lib
  )

find_library(
        log-lib
        log)

find_package(ZLIB)

target_link_libraries(
        fileaccessor
        ${log-lib}
        ${ZLIB_LIBRARIES}
      )

