#include <QApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    
    QTableWidget *table = new QTableWidget(3, 3);
    QStringList headers = {"任务名称", "任务类型", "目标类型"};
    table->setHorizontalHeaderLabels(headers);
    
    // 设置表头样式
    QString headerStyle = 
        "QHeaderView::section { "
        "background: rgba(0,120,200,0.25); "
        "border-bottom: 1px solid rgba(0,160,255,0.35); "
        "border-right: 1px solid rgba(0,140,220,0.12); "
        "padding: 5px 8px; "
        "font-size: 11px; "
        "font-weight: 500; "
        "color: rgba(120,180,230,0.6); "
        "text-align: left; "
        "letter-spacing: 0.3px; "
        "white-space: nowrap; "
        "overflow: hidden; "
        "text-overflow: ellipsis; "
        "}"
        "QHeaderView::section:horizontal:last { "
        "border-right: none; "
        "}";
    
    table->horizontalHeader()->setStyleSheet(headerStyle);
    
    // 设置表格样式
    QString tableStyle = 
        "QTableWidget { "
        "background: rgba(8,24,48,0.92); "
        "border: 1px solid rgba(0,160,255,0.35); "
        "border-radius: 4px; "
        "gridline-color: rgba(0,140,220,0.12); "
        "selection-background-color: rgba(0,180,255,0.18); "
        "selection-color: #e8f4ff; "
        "}"
        "QTableWidget::item { "
        "color: #c8e8ff; "
        "background: rgba(255,255,255,0.02); "
        "border-bottom: 1px solid rgba(0,140,220,0.12); "
        "}"
        "QTableWidget::item:hover { "
        "background: rgba(0,180,255,0.08); "
        "}";
    
    table->setStyleSheet(tableStyle);
    
    // 添加一些测试数据
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(QString("测试数据 %1-%2").arg(row+1).arg(col+1));
            table->setItem(row, col, item);
        }
    }
    
    layout->addWidget(table);
    window->setGeometry(100, 100, 500, 300);
    window->show();
    
    return app.exec();
}