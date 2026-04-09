#include "PathDisplayDialog.h"
#include "ui_PathDisplayDialog.h"
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QHeaderView>

PathDisplayDialog::PathDisplayDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PathDisplayDialog)
{
    ui->setupUi(this);

    ui->pathPointTable->setColumnCount(4);
    ui->pathPointTable->setHorizontalHeaderLabels(QStringList() << "序号" << "纬度(°)" << "经度(°)" << "高度(m)");
    ui->pathPointTable->horizontalHeader()->setStretchLastSection(true);
    ui->pathPointTable->verticalHeader()->setVisible(false);
    ui->pathPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pathPointTable->setAlternatingRowColors(false);

    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->closeButton2, &QPushButton::clicked, this, &QDialog::reject);
}

PathDisplayDialog::~PathDisplayDialog()
{
    delete ui;
}

void PathDisplayDialog::setPathData(const PathPlanning& path)
{
    ui->uavNameValue->setText(path.uavName);
    ui->taskValue->setText(path.relatedTask);
    ui->pointCountValue->setText(QString::number(path.pathPointCount));

    ui->pathPointTable->setRowCount(0);

    for (int i = 0; i < path.pathPoints.size(); ++i) {
        const PathPoint& point = path.pathPoints.at(i);
        int row = ui->pathPointTable->rowCount();
        ui->pathPointTable->insertRow(row);

        ui->pathPointTable->setItem(row, 0, new QTableWidgetItem(QString::number(point.pointOrder)));
        ui->pathPointTable->setItem(row, 1, new QTableWidgetItem(QString::number(point.latitude, 'f', 6)));
        ui->pathPointTable->setItem(row, 2, new QTableWidgetItem(QString::number(point.longitude, 'f', 6)));
        ui->pathPointTable->setItem(row, 3, new QTableWidgetItem(QString::number(point.altitude, 'f', 1)));
    }
}
