OPTION(YARS_USE_GO_GUI "Compile GO GUI Controller" OFF)
if(YARS_USE_GO_GUI)
  include(${CMAKE_SOURCE_DIR}/cmake/GolangSimple.cmake)

  set(CMAKE_Go_COMPILER "/usr/local/bin/go")
  set(GOPATH "/Users/zahedi/projects/go/")
  set(CMAKE_Go_BUILD_MODE "-buildmode=c-shared")

  GO_GET(go_redis github.com/hoisie/redis)
  ADD_GO_INSTALLABLE_PROGRAM(guiclient # executable name
                           *.go # `package main` source file
                           ) # everything else is a dependency

endif(YARS_USE_GO_GUI)