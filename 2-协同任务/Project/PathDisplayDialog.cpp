#include "PathDisplayDialog.h"
#include "ui_PathDisplayDialog.h"
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QHeaderView>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 * @details 初始化UI，设置路径点表格属性
 */
PathDisplayDialog::PathDisplayDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PathDisplayDialog)
{
    ui->setupUi(this);

    // 配置路径点表格
    ui->pathPointTable->setColumnCount(4);
    ui->pathPointTable->setHorizontalHeaderLabels(QStringList() << "序号" << "纬度(°)" << "经度(°)" << "高度(m)");
    ui->pathPointTable->horizontalHeader()->setStretchLastSection(true);
    ui->pathPointTable->verticalHeader()->setVisible(false);
    ui->pathPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pathPointTable->setAlternatingRowColors(false);

    // 连接关闭按钮信号
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->closeButton2, &QPushButton::clicked, this, &QDialog::reject);
}

/**
 * @brief 析构函数
 * @details 释放UI资源
 */
PathDisplayDialog::~PathDisplayDialog()
{
    delete ui;
}

/**
 * @brief 设置路径数据
 * @param path 路径规划信息
 * @details 显示无人机名称、关联任务、路径点数，并将路径点添加到表格
 */
void PathDisplayDialog::setPathData(const PathPlanning& path)
{
    // 显示基本信息
    ui->uavNameValue->setText(path.uavName);
    ui->taskValue->setText(path.relatedTask);
    ui->pointCountValue->setText(QString::number(path.pathPointCount));

    // 清空并重新填充路径点表格
    ui->pathPointTable->setRowCount(0);

    for (int i = 0; i < path.pathPoints.size(); ++i) {
        const PathPoint& point = path.pathPoints.at(i);
        int row = ui->pathPointTable->rowCount();
        ui->pathPointTable->insertRow(row);

        // 序号
        ui->pathPointTable->setItem(row, 0, new QTableWidgetItem(QString::number(point.pointOrder)));
        // 纬度（保留6位小数）
        ui->pathPointTable->setItem(row, 1, new QTableWidgetItem(QString::number(point.latitude, 'f', 6)));
        // 经度（保留6位小数）
        ui->pathPointTable->setItem(row, 2, new QTableWidgetItem(QString::number(point.longitude, 'f', 6)));
        // 高度（保留1位小数）
        ui->pathPointTable->setItem(row, 3, new QTableWidgetItem(QString::number(point.altitude, 'f', 1)));
    }
}
