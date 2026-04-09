#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>
#include "SourceRadiation.h"
#include "FirepowerControl.h"
#include "SituationControl.h"
#include "RZSourceRadiation.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置全局样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 创建并显示主窗口
    RZSourceRadiation w;
    w.show();
    // SituationControl window;
    // window.show();
    // FirepowerControl firepower;
    // firepower.show();


    return app.exec();
}