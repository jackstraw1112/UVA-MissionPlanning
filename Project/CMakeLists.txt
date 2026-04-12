# 设置cmake最低版本
cmake_minimum_required(VERSION 3.16)

# =================================================================================
# Qt C++ 相关配置
# =================================================================================

# 设置自动MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#根据运行平台 设置QT安装路径
if(WIN32)
    set(QT_DIR "C:/Qt/Qt5.12.12/5.12.12/msvc2017_64")
else ()
    set(QT_DIR "/usr/local/bin/Qt5.9.0/bin/gcc_64")
endif ()
# 指定Qt 安装路径
set(CMAKE_PREFIX_PATH ${QT_DIR})

# 明确启用 C++ 支持
#enable_language(CXX)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
# 强制要求使用指定的C++标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置编译文件默认为：UTF-8 编码
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

# 设置对象文件路径最大长度限制
set(CMAKE_OBJECT_PATH_MAX 1000)
# 设置SDK版本
# set(CMAKE_SYSTEM_VERSION 10.0.19041.0)
# 设置CMake模块路径
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 添加编译类型预设
#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE "Release")
#endif()

# =================================================================================
# 项目公共配置
# =================================================================================

# 设置工程名称
project(UVA-MissionPlanning VERSION 1.0.0 LANGUAGES CXX)

# 查找Qt依赖[1,5](@ref)
find_package(Qt5 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Sql
        Charts
)

# 设置输出目录
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set_target_properties(${PROJECT_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_x86_64"        # app输出目录
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_x86_64/lib"    # so输出目录
        )
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set_target_properties(${PROJECT_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_x86_64"        # app输出目录
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_x86_64/lib"    # so输出目录
        )
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "ARM64")
        set_target_properties(${PROJECT_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_aarch64"        # app输出目录
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_aarch64/lib"    # so输出目录
        )
    else ()
        set_target_properties(${PROJECT_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_loongarch64"        # app输出目录
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin_linux_loongarch64/lib"    # so输出目录
        )
    endif ()
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin_windows)         # app输出目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)                 # lib输出目录
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin_windows/plugins) # dll输出目录
endif ()

# 设置数据目录变量：OUTPUT_DIR
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(OUTPUT_DIR "bin_linux_x64")
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(OUTPUT_DIR "bin_linux_x64")
    elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "ARM64")
        set(OUTPUT_DIR "bin_linux_aarch64")
    else ()
        set(OUTPUT_DIR "bin_linux_loongarch64")
    endif ()
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(OUTPUT_DIR "bin_Windows")
endif ()

# 设置二进制文件、库文件、归档文件的统一输出目录
#set(CMAKE_BINARY_DIR ${OUTPUT_DIR}/compile/${PROJECT_NAME})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 配置 Qt 工具（moc/rcc/uic）的生成文件存放目录
#set(CMAKE_MOC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/moc)
#set(CMAKE_RCC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rcc)
#set(CMAKE_UIC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ui)

# 添加子目录
add_subdirectory(src/EstimateSituation)