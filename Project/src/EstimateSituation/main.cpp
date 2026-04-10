//
// Created by admin on "2026.04.09 T 19:19:20".
//

#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>

#include "FirepowerControl.h"
#include "SituationControl.h"
#include "RZSourceRadiation.h"
#include "ThreatAssessment.h"

/**
 * @brief 主函数
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 应用程序退出码
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置全局样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 创建并显示主窗口
    RZSourceRadiation w;
    w.show();
    SituationControl window;
    window.show();
    FirepowerControl firepower;
    firepower.show();
    ThreatAssessment t;
    t.show();

    return app.exec();
}