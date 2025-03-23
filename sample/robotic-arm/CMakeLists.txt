option(BUILD_ROBOTIC_ARM_SAMPLES "Building Robotic arm samples" ON)
if(NOT BUILD_ROBOTIC_ARM_SAMPLES)
  return()
endif()

if(UNIX)
  add_custom_target(
    symlink_to_RoboticArm ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR} ${CHOREONOID_BINARY_SHARE_DIR}/RoboticArm)
endif()

install(DIRECTORY model DESTINATION ${CHOREONOID_SHARE_SUBDIR}/RoboticArm)

add_subdirectory(sample)