project(yarsClientCom Java)

set(FILES Braitenberg)

set(JAVA_FILES)
foreach(f ${FILES})
  set(JAVA_FILES ${JAVA_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.java)
endforeach(f ${FILES})

set(JAR_NAME   "braitenberg")
set(DIST_DIR   "jar")
set(BUILD_DIR  "${CMAKE_CURRENT_BINARY_DIR}/build")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MAIN_CLASS "org.yars.examples.Braitenberg")
set(CLASS_PATH "yarsclientcom.jar")
set(ANT_COMMAND ant)

configure_file(${CMAKE_SOURCE_DIR}/contrib/javaClientCom/build.xml
    ${CMAKE_CURRENT_BINARY_DIR}/build.xml @ONLY)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/jar/${JAR_NAME}.jar
    DEPENDS ${JAVA_FILES}
    DEPENDS ${JAVA_FILES} ${CMAKE_BINARY_DIR}/jar/yarsclientcom.jar
    COMMAND ${CMAKE_COMMAND}
    ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${ANT_COMMAND} distribute)

add_custom_target(${JAR_NAME}.jar
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/jar/${JAR_NAME}.jar)

