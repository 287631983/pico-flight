add_library(flight INTERFACE)

target_sources(flight INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/flight_attitude.c
        ${CMAKE_CURRENT_LIST_DIR}/src/flight_controller.c
        ${CMAKE_CURRENT_LIST_DIR}/src/fc_vars.c
)

target_include_directories(flight INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/src  
)

target_link_libraries(flight INTERFACE
    pico_multicore
    core-system
    core-math
    core-telemetry-common
    data-vars
    motor-common
    motor-mixer
    motor-output
    io_rc
    gyro
    rp2040_osd
    hardware_watchdog
    # web_server
    # camera
    )