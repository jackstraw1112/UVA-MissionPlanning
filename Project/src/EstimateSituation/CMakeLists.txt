# 设置cmake最低版本
cmake_minimum_required(VERSION 3.16)

# 设置工程名称以及版本
project(EstimateSituation)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 确保使用正确的 C++ 标准库
if(MSVC)
    # MSVC 会自动根据 CMAKE_CXX_STANDARD 设置选择正确的标准
endif()

# 启用 Qt 自动生成
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# =================================================================================
# 设置：包含目录与库目录
# =================================================================================

# 查找Qt软件包（需要在include_directories之前）
find_package(Qt5        # 查找 Qt5 库
        REQUIRED        # 表示必需找到否则构建失败
        COMPONENTS      # 指定需要的 Qt 模块组件
        Core
        Gui
        Widgets
        Charts
)

# 添加包含目录-方法1
#include_directories(
#        ${CMAKE_SOURCE_DIR}/include/RZ_DataPack
#        ${CMAKE_SOURCE_DIR}/include/RZCommon
#        ${CMAKE_SOURCE_DIR}/include/RZJsonParser
#)

# 添加链接库目录
#if (WIN32)
#    link_directories(${CMAKE_SOURCE_DIR}/lib)
#else ()
#    #linux下只存在so文件且一般在bin/lib目录
#    link_directories(${CMAKE_SOURCE_DIR}/${OUTPUT_DIR}/lib)
#endif ()

# =================================================================================
# 创建共享库以及设置共享库编译源文件-方法1
# =================================================================================

# 创建共享库（指定动态库名称 + 指定构建类型为SHARED动态库）
#add_library(${PROJECT_NAME} SHARED)
add_executable(${PROJECT_NAME} WIN32)

# 自动收集源文件和头文件，减少手动维护
# 注意：GLOB 不适用于需要精确控制文件顺序的场景
file(GLOB SOURCES
        "*.cpp"
)

file(GLOB HEADERS
        "*.h"
)

# 收集 UI 文件
file(GLOB UI_FILES
        "*.ui"
)

# 添加资源文件
#qt5_add_resource(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/xxx.qrc)

# 添加图标
#set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/xxx.rc)

# 增加文件存在性校验，提高健壮性
foreach(SRC_FILE IN LISTS SOURCES)
    if(NOT EXISTS "${SRC_FILE}")
        message(FATAL_ERROR "Source file not found: ${SRC_FILE}")
    endif()
endforeach()
foreach(HEADER_FILE IN LISTS HEADERS)
    if(NOT EXISTS "${HEADER_FILE}")
        message(FATAL_ERROR "Header file not found: ${HEADER_FILE}")
    endif()
endforeach()

# 将源文件和头文件添加到指定的目标库中
target_sources(${PROJECT_NAME} PRIVATE
        ${SOURCES}
        ${HEADERS}
#        ${RESOURCE}
#        ${ICON}
)

# =================================================================================
# 设置目标属性
# =================================================================================

# 设置目标输出名称
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "EstimateSituation"
)

# =================================================================================
# 项目配置
# =================================================================================

# 指定目标需要链接的库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
)

# =================================================================================
# 调试相关
# =================================================================================

# 设置Release调试-方法1-仅在当前工程使用时有效
set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Zi /Ob0 /Od /DNDEBUG /FS")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /Ob0 /Od /DNDEBUG /FS")

# 添加 /FS 选项以解决 PDB 文件冲突
if(MSVC)
    add_compile_options(/FS)
endif()

