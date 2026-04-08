QT       += core widgets

TARGET = CooperativeMissionPlanning
TEMPLATE = app

# 设置C++标准
CONFIG += c++17

# 源文件列表
SOURCES += \
    main.cpp \
    CooperativeMissionPlan.cpp \
    taskdialog.cpp

# 头文件列表
HEADERS += \
    CooperativeMissionPlan.h \
    taskdialog.h \
    StructData.h

# UI文件列表
FORMS += \
    CooperativeMissionPlan.ui \
    taskdialog.ui

# 资源文件
RESOURCES += \
    resources.qrc

# 编译选项
QMAKE_CXXFLAGS += /utf-8

# 输出目录
DESTDIR = ./build
MOC_DIR = ./build/moc
RCC_DIR = ./build/rcc
UI_DIR = ./build/ui
OBJECTS_DIR = ./build/obj

# 安装设置
win32 {
    QMAKE_POST_LINK += $$quote(cmd /c "mkdir -p $${DESTDIR}")
}
