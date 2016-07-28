add_executable(braitenberg-pipe Braitenberg.cpp)

target_link_libraries(braitenberg-pipe YarsNamedPipeCom
                                       YarsRobotController
                                       YarsConfiguration
                                       YarsUtil
                                       ${Boost_LIBRARIES})

