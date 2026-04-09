cmake_minimum_required(VERSION 3.16)
project(SituationAnalysis VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置Qt路径
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12")

# Visual Studio设置
if(MSVC)
    # UTF-8编码
    add_compile_options(/utf-8)
    # 并行编译
    add_compile_options(/MP)
endif()

# 查找Qt库
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# 源文件
set(SOURCES
    main.cpp
    SituationAnalysis.cpp
)

# 头文件
set(HEADERS
    SituationAnalysis.h
)

# 资源
set(RESOURCES
    resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
)

# Windows设置
if(WIN32 AND MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()
