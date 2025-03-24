option(BUILD_UGV_SAMPLES "Building UGV samples" ON)
if(NOT BUILD_UGV_SAMPLES)
  return()
endif()

if(UNIX)
  add_custom_target(
    symlink_to_UGV ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR} ${CHOREONOID_BINARY_SHARE_DIR}/UGV)
endif()

install(DIRECTORY model DESTINATION ${CHOREONOID_SHARE_SUBDIR}/UGV)

add_subdirectory(sample)