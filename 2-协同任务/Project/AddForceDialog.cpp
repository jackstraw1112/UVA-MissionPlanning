#include "AddForceDialog.h"
#include "ui_AddForceDialog.h"
#include <QMessageBox>

AddForceDialog::AddForceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddForceDialog)
{
    ui->setupUi(this);
    
    connect(ui->okButton, &QPushButton::clicked, this, &AddForceDialog::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddForceDialog::onCancelButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &AddForceDialog::onCloseButtonClicked);
}

AddForceDialog::~AddForceDialog()
{
    delete ui;
}

void AddForceDialog::onOkButtonClicked()
{
    QString groupName = ui->groupNameEdit->text().trimmed();
    QString equipmentType = ui->equipmentComboBox->currentText();
    int maxCount = ui->maxCountEdit->text().toInt();
    int initialCount = ui->initialCountEdit->text().toInt();
    
    if (groupName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入编组名称");
        return;
    }
    
    if (initialCount > maxCount) {
        QMessageBox::warning(this, "提示", "初始配置数量不能超过上限");
        return;
    }
    
    GroupInfo newGroup;
    newGroup.groupName = groupName;
    
    EquipmentType newEquipment;
    newEquipment.equipmentName = equipmentType;
    
    for (int i = 0; i < initialCount; ++i) {
        UAVInfo uav;
        uav.uavName = equipmentType.split(" ").at(0) + QString("-%1").arg(i + 1);
        uav.status = "待命";
        newEquipment.uavList.append(uav);
    }
    
    newGroup.equipmentList.append(newEquipment);
    
    emit forceAdded(newGroup);
    accept();
}

void AddForceDialog::onCancelButtonClicked()
{
    reject();
}

void AddForceDialog::onCloseButtonClicked()
{
    reject();
}
