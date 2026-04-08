#include "SituationAnalysis.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("SituationAnalysis");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("UVA-MissionPlanning");
    
    // 设置全局样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 创建并显示主窗口
    SituationAnalysis window;
    window.show();
    
    return app.exec();
}