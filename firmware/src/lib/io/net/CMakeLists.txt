# add_subdirectory(test)
if(NOT PF_BUILD_TOOLS)
  add_subdirectory(lwip)
  add_subdirectory(usb_network_stack)
  add_subdirectory(web)
endif()