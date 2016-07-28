project(yarsClient)

set(GUI_HDRS YarsClientMainWidget.h
             ControlWidget.h
             YarsControlWidget.h
             CanvasWidget.h
             StatusBarControl.h
             YarsClientControl.h
             CommunicationThread.h
             DataPanel.h
             SelectionPanel.h
             DataContentPanel.h
             SelectionPanel.h
             NumbersPanel.h
             MotorPanel.h
             SensorPanel.h
             InputPanel.h)

QT4_WRAP_CPP(qtview_MOC_SRCS ${GUI_HDRS})

set(SRCS     ${qtview_MOC_SRCS}
             yarsClientMain.cpp
             YarsClientMainWidget.cpp
             ControlWidget.cpp
             YarsControlWidget.cpp
             CanvasWidget.cpp
             InputPanel.cpp
             CommunicationModel.cpp
             CommunicationThread.cpp
             YarsClientControl.cpp
             SelectionPanel.cpp
             SelectionPanel.cpp
             SensorPanel.cpp
             MotorPanel.cpp
             DataContentPanel.cpp
             NumbersPanel.cpp
             DataPanel.cpp
             StatusBarControl.cpp)

set(EXECUTABLE_SETTING)

if(APPLE)
  set(EXECUTABLE_SETTING MACOSX_BUNDLE)
endif(APPLE)

if(WIN32)
  set(EXECUTABLE_SETTING WIN32)
endif(WIN32)

if(UNIX AND NOT APPLE)
  set(EXECUTABLE_SETTING "")
endif(UNIX AND NOT APPLE)

add_executable(yars-client ${EXECUTABLE_SETTING} ${SRCS})
add_dependencies(yars-client YarsClientCom)

include_directories(${CMAKE_SOURCE_DIR}/contrib/cppClientCom)
target_link_libraries(yars-client ${QT_LIBRARIES}
                                  YarsClientCom)
