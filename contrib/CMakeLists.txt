if(YARS_BUILD_CPP_CLIENT)
  add_subdirectory(braitenbergCom)
  add_subdirectory(braitenbergPipe)
  add_subdirectory(namedPipeClientCom)
  add_subdirectory(cppClientCom)
  #if(YARS_USE_VISUALISATION)
    #add_subdirectory(yarsClient)
  #endif(YARS_USE_VISUALISATION)
endif(YARS_BUILD_CPP_CLIENT)

if(YARS_BUILD_JAVA_CLIENT)
  add_subdirectory(javaClientCom)
  add_subdirectory(javaBraitenberg)
endif(YARS_BUILD_JAVA_CLIENT)


add_subdirectory(controller)
add_subdirectory(ctrlGui)
