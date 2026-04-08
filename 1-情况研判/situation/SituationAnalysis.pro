QT       += core widgets

TARGET = SituationAnalysis
TEMPLATE = app

# 设置C++标准
CONFIG += c++17

# 源文件
SOURCES += main.cpp \
           SituationAnalysis.cpp

# 头文件
HEADERS += SituationAnalysis.h

# UI文件
FORMS += SituationAnalysis.ui

# 资源文件
RESOURCES += resources.qrc

# 设置字符编码
win32: {
    QMAKE_CXXFLAGS += /utf-8
}

# 设置Qt版本
QT_VERSION = 5.12.12

# 设置输出目录
DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui