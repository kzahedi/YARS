if(UNIX)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/xml       ${CMAKE_BINARY_DIR}/xml)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/contrib/py  ${CMAKE_BINARY_DIR}/py)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/ply         ${CMAKE_BINARY_DIR}/ply)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/scripts     ${CMAKE_BINARY_DIR}/scripts)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/contrib/controller     ${CMAKE_BINARY_DIR}/controller)
endif(UNIX)
