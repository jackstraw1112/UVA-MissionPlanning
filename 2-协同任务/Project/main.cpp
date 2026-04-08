#include "CooperativeMissionPlan.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application style
    a.setStyle(QStyleFactory::create("Fusion"));
    
    // Set application information
    QApplication::setApplicationName("无人机任务规划与效能评估系统");
    QApplication::setOrganizationName("军事仿真实验室");
    QApplication::setApplicationVersion("1.0.0");
    
    CooperativeMissionPlan w;
    w.show();
    return a.exec();
}