
add_library(YarsLogging ${LIBRARY_TYPE} YarsLoggingModel.cpp

                                        LoggingHandler.cpp
                                        LoggingModule.cpp
                                        LoggingModuleObject.cpp
                                        LoggingModuleController.cpp
                                        LoggingModuleSensor.cpp
                                        LoggingModuleActuator.cpp

                                        Logger.cpp
                                        FileLogger.cpp
                                        CSVLogger.cpp
                                        GnuplotLogger.cpp
                                        SelforgLogger.cpp
                                        BlenderLogger.cpp
                                        ConsoleLogger.cpp)
