add_executable(braitenberg-tcpip Braitenberg.cpp)

target_link_libraries(braitenberg-tcpip YarsClientCom
                                  YarsRobotController
                                  YarsConfiguration
                                  YarsUtil
                                  pthread
                                  ${Boost_LIBRARIES})

