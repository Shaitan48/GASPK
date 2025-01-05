cmake_minimum_required(VERSION 3.14)

project(QtClientServerWithQML)

# Указываем, что проект использует Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/common)
# Указываем файлы исходного кода для клиента и сервера
set(CLIENT_SOURCES
    src/client/main.cpp
    src/client/Client.cpp
    src/client/MainWindow.cpp
)

set(SERVER_SOURCES
    src/server/main.cpp
    src/server/Server.cpp
)

set(AGENT_SOURCES
    src/agent/main.cpp
)

set(COMMON_SOURCES
    src/common/Trigger.cpp
    src/common/Operation.cpp
    src/common/Task.cpp
    src/common/TriggerWatchDog.cpp
    src/common/PingTrigger.cpp
    src/common/DiskSpaceTrigger.cpp
     src/common/FileDeleteOperation.cpp
     src/common/FileCopyOperation.cpp
     src/common/FileMoveOperation.cpp
     src/common/MessageOperation.cpp
     src/common/RequestOperation.cpp
)

set(CMAKE_AUTOMOC ON)
qt_wrap_cpp(SERVER_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/server/Server.h)
qt_wrap_cpp(TRIGGER_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Trigger.h)
qt_wrap_cpp(OPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Operation.h)
qt_wrap_cpp(TASK_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Task.h)
qt_wrap_cpp(TRIGGERWATCHDOG_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/TriggerWatchDog.h)
qt_wrap_cpp(CLIENT_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/client/Client.h)
qt_wrap_cpp(AGENT_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/agent/Agent.h)
qt_wrap_cpp(PINGTRIGGER_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/PingTrigger.h)
qt_wrap_cpp(DISKSPACETRIGGER_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/DiskSpaceTrigger.h)
qt_wrap_cpp(FILEDELETEOPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/FileDeleteOperation.h)
qt_wrap_cpp(FILECOPYOPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/FileCopyOperation.h)
qt_wrap_cpp(FILEMOVEOPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/FileMoveOperation.h)
qt_wrap_cpp(MESSAGEOPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/MessageOperation.h)
qt_wrap_cpp(REQUESTOPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/RequestOperation.h)
qt_wrap_cpp(MAINWINDOW_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/client/MainWindow.h)
qt_wrap_cpp(TASK_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Task.h)


# Создаем исполнимые файлы для сервера и клиента
add_executable(Client ${CLIENT_SOURCES}
    src/client/Client.h
    ${CLIENT_HEADERS_MOC}
    src/client/MainWindow.h
    ${MAINWINDOW_HEADERS_MOC}
    src/common/Trigger.h
    ${TRIGGER_HEADERS_MOC}
    src/common/Operation.h
    ${OPERATION_HEADERS_MOC}
    src/common/Task.h
    ${TASK_HEADERS_MOC}
    src/common/TriggerWatchDog.h
    ${TRIGGERWATCHDOG_HEADERS_MOC}
    ${COMMON_SOURCES}
     src/common/PingTrigger.h
    ${PINGTRIGGER_HEADERS_MOC}
    src/common/DiskSpaceTrigger.h
    ${DISKSPACETRIGGER_HEADERS_MOC}
     src/common/FileDeleteOperation.h
    ${FILEDELETEOPERATION_HEADERS_MOC}
      src/common/FileCopyOperation.h
    ${FILECOPYOPERATION_HEADERS_MOC}
      src/common/FileMoveOperation.h
    ${FILEMOVEOPERATION_HEADERS_MOC}
      src/common/MessageOperation.h
    ${MESSAGEOPERATION_HEADERS_MOC}
      src/common/RequestOperation.h
    ${REQUESTOPERATION_HEADERS_MOC}

)
add_executable(Server ${SERVER_SOURCES}
    src/server/Server.h
    ${SERVER_HEADERS_MOC}
    src/common/Trigger.h
    ${TRIGGER_HEADERS_MOC}
    src/common/Operation.h
    ${OPERATION_HEADERS_MOC}
    src/common/Task.h
    ${TASK_HEADERS_MOC}
     src/common/TriggerWatchDog.h
     ${TRIGGERWATCHDOG_HEADERS_MOC}
      ${COMMON_SOURCES}
       src/common/PingTrigger.h
    ${PINGTRIGGER_HEADERS_MOC}
    src/common/DiskSpaceTrigger.h
    ${DISKSPACETRIGGER_HEADERS_MOC}
      src/common/FileDeleteOperation.h
    ${FILEDELETEOPERATION_HEADERS_MOC}
      src/common/FileCopyOperation.h
    ${FILECOPYOPERATION_HEADERS_MOC}
      src/common/FileMoveOperation.h
    ${FILEMOVEOPERATION_HEADERS_MOC}
      src/common/MessageOperation.h
    ${MESSAGEOPERATION_HEADERS_MOC}
      src/common/RequestOperation.h
    ${REQUESTOPERATION_HEADERS_MOC}
)

add_executable(Agent ${AGENT_SOURCES}
    src/agent/Agent.h
    ${AGENT_HEADERS_MOC}
    src/agent/Agent.cpp
     src/common/Trigger.h
    ${TRIGGER_HEADERS_MOC}
    src/common/Operation.h
    ${OPERATION_HEADERS_MOC}
    src/common/Task.h
    ${TASK_HEADERS_MOC}
    src/common/TriggerWatchDog.h
    ${TRIGGERWATCHDOG_HEADERS_MOC}
     ${COMMON_SOURCES}
      src/common/PingTrigger.h
    ${PINGTRIGGER_HEADERS_MOC}
    src/common/DiskSpaceTrigger.h
    ${DISKSPACETRIGGER_HEADERS_MOC}
      src/common/FileDeleteOperation.h
    ${FILEDELETEOPERATION_HEADERS_MOC}
      src/common/FileCopyOperation.h
    ${FILECOPYOPERATION_HEADERS_MOC}
      src/common/FileMoveOperation.h
    ${FILEMOVEOPERATION_HEADERS_MOC}
      src/common/MessageOperation.h
    ${MESSAGEOPERATION_HEADERS_MOC}
      src/common/RequestOperation.h
    ${REQUESTOPERATION_HEADERS_MOC}
)


# Указываем, что проект использует Qt6
target_link_libraries(Client Qt6::Core Qt6::Widgets Qt6::Network Qt6::Sql)
target_link_libraries(Server Qt6::Core Qt6::Network Qt6::Sql)
target_link_libraries(Agent Qt6::Core Qt6::Network)

set_target_properties(Agent PROPERTIES WIN32_EXECUTABLE TRUE)

 add_custom_command(TARGET Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/src/server/config.json
        $<TARGET_FILE_DIR:Server>)


# Включаем модули Qt для CMake
qt_standard_project_setup()
