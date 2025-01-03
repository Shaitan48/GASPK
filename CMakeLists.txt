cmake_minimum_required(VERSION 3.14)

project(QtClientServerWithQML)

# Указываем, что проект использует Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Quick Network)

# Указываем файлы исходного кода для клиента и сервера
set(CLIENT_SOURCES
    src/client/main.cpp
    src/client/Client.cpp
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
)

set(CMAKE_AUTOMOC ON)
qt_wrap_cpp(SERVER_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/server/Server.h)
qt_wrap_cpp(TRIGGER_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Trigger.h)
qt_wrap_cpp(OPERATION_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Operation.h)
qt_wrap_cpp(TASK_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/Task.h)
qt_wrap_cpp(TRIGGERWATCHDOG_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/common/TriggerWatchDog.h)
qt_wrap_cpp(CLIENT_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/client/Client.h)
qt_wrap_cpp(AGENT_HEADERS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/src/agent/Agent.h)

# Создаем исполнимые файлы для сервера и клиента
add_executable(Client ${CLIENT_SOURCES}
    src/client/Client.h
    ${CLIENT_HEADERS_MOC}
    src/client/qml/MainWindow.qml
    src/common/Trigger.h
    ${TRIGGER_HEADERS_MOC}
    src/common/Operation.h
    ${OPERATION_HEADERS_MOC}
    src/common/Task.h
    ${TASK_HEADERS_MOC}
     src/common/TriggerWatchDog.h
    ${TRIGGERWATCHDOG_HEADERS_MOC}
    ${COMMON_SOURCES}
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
    ${COMMON_SOURCES})

target_include_directories(Server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
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
)

# Указываем, что проект использует Qt6
target_link_libraries(Client Qt6::Core Qt6::Quick Qt6::Network)
target_link_libraries(Server Qt6::Core Qt6::Network)
target_link_libraries(Agent Qt6::Core Qt6::Network)

set_target_properties(Agent PROPERTIES WIN32_EXECUTABLE TRUE)

 add_custom_command(TARGET Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/src/server/config.json
        $<TARGET_FILE_DIR:Server>)

add_custom_command(TARGET Client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/src/client/qml/MainWindow.qml
        $<TARGET_FILE_DIR:Client>)

# Включаем модули Qt для CMake
qt_standard_project_setup()
