#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>
#include "SourceRadiation.h"
#include "situationcontrol.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置全局样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 创建并显示主窗口
    SourceRadiation w;
    w.show();
    // SituationControl window;
    // window.show();

    return app.exec();
}