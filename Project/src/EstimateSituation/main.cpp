//
// Created by admin on "2026.04.09 T 19:19:20".
//

#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>
#include "SpectrumAnalysis.h"
#include "EstimateSituation.h"
#include "RZThreatAssessment.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置全局样式
    app.setStyle(QStyleFactory::create("Fusion"));

    // 创建并显示主窗口
    RZThreatAssessment w;
    w.show();
    // SpectrumAnalysis s;
    // s.show();


    return app.exec();
}
