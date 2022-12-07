cmake_minimum_required(VERSION 3.13)

add_executable(KnVnM_Host)

target_sources(KnVnM_Host PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/hid_app.c
        ${CMAKE_CURRENT_LIST_DIR}/msc_app.c
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

target_include_directories(KnVnM_Host PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(KnVnM_Host PUBLIC pico_stdlib pico_multicore tinyusb_host tinyusb_board)


# disable uart output
# pico_enable_stdio_uart(KnVnM_Host 0)

pico_add_extra_outputs(KnVnM_Host)
