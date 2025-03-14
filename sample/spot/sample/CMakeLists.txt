choreonoid_add_simple_controller(SpotArmController SpotArmController.cpp)
choreonoid_add_simple_controller(SpotLiedownController SpotLiedownController.cpp)

install(FILES SpotArm.cnoid SpotLiedown.cnoid
    DESTINATION ${CHOREONOID_SHARE_SUBDIR}/project)