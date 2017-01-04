add_executable(braitenberg-pipe Braitenberg.cpp)

target_link_libraries(braitenberg-pipe YarsUtil
                                       YarsNamedPipeCom
                                       YarsRobotController
                                       YarsConfiguration
                                       ${Boost_LIBRARIES})

