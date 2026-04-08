#include "SituationAnalysis.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>
#include "SourceRadiation.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置全局样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 创建并显示主窗口
    SourceRadiation window;
    window.show();
    
    return app.exec();
}