if(NOT PF_BUILD_TOOLS)
  add_executable(pico-flight
          pico-flight.c
          )

  target_include_directories(pico-flight PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR})

  # Pull in our pico_stdlib which aggregates commonly used features
  target_link_libraries(pico-flight 
      # pico_stdlib
      # pico_stdio_uart
      flight
      core-system-rp2040
      core-telemetry-rp2040
      motor-output-dshot-rp2040-pio
    )

  # enable usb output, disable uart output
  if(PICO_NO_HARDWARE)
    pico_enable_stdio_usb(pico-flight 0)
    pico_enable_stdio_uart(pico-flight 0)
    pico_enable_stdio_semihosting(pico-flight 1)
  else()
  
    target_link_libraries( pico-flight 
      pico_stdlib
      pico_stdio_uart
    )
  endif()
  # create map/bin/hex/uf2 file etc.
  pico_add_extra_outputs(pico-flight)

  pico_enable_stdio_usb(pico-flight 1)
  # add url via pico_set_program_url
  #example_auto_set_url(pico-flight)

endif()

add_subdirectory(lib)

