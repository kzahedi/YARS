add_library(YarsUtil ${LIBRARY_TYPE} filter/MovingAverage.cpp
                                     PID.cpp
                                     Filter.cpp
                                     StringTokeniser.cpp
                                     VideoCodecs.cpp
                                     Random.cpp
                                     Socket.cpp
                                     NamedPipe.cpp
                                     PlyLoader.cpp
                                     Mapping.cpp
                                     YarsErrorHandler.cpp
                                     MappingFunction.cpp
                                     noise/GaussianNoise.cpp
                                     noise/NoNoise.cpp
                                     noise/WhiteNoise.cpp
                                     Directories.cpp
                                     FileSystemOperations.cpp)

target_link_libraries(YarsUtil boost_filesystem)
