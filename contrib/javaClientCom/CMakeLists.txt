project(yarsClientCom Java)

set(FILES Buffer
          Entity
          Sensor
          Motor
          Domain
          CommunicationException
          LowLevelSocketCommunication
          YarsClientCom)

set(JAVA_FILES)
foreach(f ${FILES})
  set(JAVA_FILES ${JAVA_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.java)
endforeach(f ${FILES})

set(JAR_NAME   "yarsclientcom")
set(DIST_DIR   "jar")
set(BUILD_DIR  "${CMAKE_CURRENT_BINARY_DIR}/build")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MAIN_CLASS "")
set(CLASS_PATH "")
set(ANT_COMMAND ant)

configure_file(${CMAKE_SOURCE_DIR}/contrib/javaClientCom/build.xml
    ${CMAKE_CURRENT_BINARY_DIR}/build.xml @ONLY)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/jar/${JAR_NAME}.jar
    DEPENDS ${JAVA_FILES}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${ANT_COMMAND} distribute)

add_custom_target(${JAR_NAME}.jar
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/jar/${JAR_NAME}.jar)


