option(BUILD_NARREC_SAMPLES "Building NARREC mock-up samples" ON)
if(NOT BUILD_NARREC_SAMPLES)
  return()
endif()

if(UNIX)
  add_custom_target(
    symlink_to_NARREC ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR} ${CHOREONOID_BINARY_SHARE_DIR}/NARREC)
endif()

install(DIRECTORY model DESTINATION ${CHOREONOID_SHARE_SUBDIR}/NARREC)

add_subdirectory(sample)