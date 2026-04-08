#include "taskdialog.h"
#include "ui_taskdialog.h"
#include <QTimeEdit>
#include <QMessageBox>

TaskDialog::TaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskDialog)
{
    ui->setupUi(this);
    
    setWindowTitle("协同任务设置");
    setModal(true);
    
    // Connect buttons
    connect(ui->okButton, &QPushButton::clicked, this, &TaskDialog::onOkClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TaskDialog::onCancelClicked);
    
    // Populate task types
    ui->taskTypeCombo->addItems({"打击", "压制", "侦察", "干扰"});
    
    // Populate target types
    ui->targetTypeCombo->addItems({"点目标", "区域目标", "线目标"});
    
    // Populate target names
    ui->targetNameCombo->addItems({"E-001", "E-002", "E-003", "E-004", "E-005", "ZONE-A", "ZONE-B"});
    
    // Set default times
    ui->startTimeEdit->setTime(QTime(8, 30));
    ui->endTimeEdit->setTime(QTime(9, 15));
}

TaskDialog::~TaskDialog()
{
    delete ui;
}

QString TaskDialog::taskName() const
{
    return ui->taskNameEdit->text();
}

QString TaskDialog::taskType() const
{
    return ui->taskTypeCombo->currentText();
}

QString TaskDialog::targetType() const
{
    return ui->targetTypeCombo->currentText();
}

QString TaskDialog::targetName() const
{
    return ui->targetNameCombo->currentText();
}

QString TaskDialog::assignedUAVs() const
{
    return ui->uavsEdit->text();
}

QTime TaskDialog::startTime() const
{
    return ui->startTimeEdit->time();
}

QTime TaskDialog::endTime() const
{
    return ui->endTimeEdit->time();
}

void TaskDialog::setTaskName(const QString& name)
{
    ui->taskNameEdit->setText(name);
}

void TaskDialog::setTaskType(const QString& type)
{
    int index = ui->taskTypeCombo->findText(type);
    if (index >= 0) {
        ui->taskTypeCombo->setCurrentIndex(index);
    }
}

void TaskDialog::setTargetType(const QString& type)
{
    int index = ui->targetTypeCombo->findText(type);
    if (index >= 0) {
        ui->targetTypeCombo->setCurrentIndex(index);
    }
}

void TaskDialog::setTargetName(const QString& name)
{
    int index = ui->targetNameCombo->findText(name);
    if (index >= 0) {
        ui->targetNameCombo->setCurrentIndex(index);
    }
}

void TaskDialog::setAssignedUAVs(const QString& uavs)
{
    ui->uavsEdit->setText(uavs);
}

void TaskDialog::setStartTime(const QTime& time)
{
    ui->startTimeEdit->setTime(time);
}

void TaskDialog::setEndTime(const QTime& time)
{
    ui->endTimeEdit->setTime(time);
}

void TaskDialog::onOkClicked()
{
    if (taskName().isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入任务名称");
        return;
    }
    
    if (assignedUAVs().isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入分配的无人机");
        return;
    }
    
    if (startTime() >= endTime()) {
        QMessageBox::warning(this, "警告", "开始时间必须早于结束时间");
        return;
    }
    
    accept();
}

void TaskDialog::onCancelClicked()
{
    reject();
}