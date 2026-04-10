#include "AddForceDialog.h"
#include "ui_AddForceDialog.h"
#include <QMessageBox>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 * @details 初始化UI，连接按钮信号槽
 */
AddForceDialog::AddForceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddForceDialog)
{
    ui->setupUi(this);

    // 连接按钮信号槽
    connect(ui->okButton, &QPushButton::clicked, this, &AddForceDialog::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddForceDialog::onCancelButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &AddForceDialog::onCloseButtonClicked);
}

/**
 * @brief 析构函数
 * @details 释放UI资源
 */
AddForceDialog::~AddForceDialog()
{
    delete ui;
}

/**
 * @brief 确定按钮处理
 * @details 获取用户输入的编组信息，验证数据有效性，创建编组并发送forceAdded信号
 */
void AddForceDialog::onOkButtonClicked()
{
    // 获取用户输入
    QString groupName = ui->groupNameEdit->text().trimmed();
    QString equipmentType = ui->equipmentComboBox->currentText();
    int maxCount = ui->maxCountEdit->text().toInt();
    int initialCount = ui->initialCountEdit->text().toInt();

    // 验证编组名称
    if (groupName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入编组名称");
        return;
    }

    // 验证数量范围
    if (initialCount > maxCount) {
        QMessageBox::warning(this, "提示", "初始配置数量不能超过上限");
        return;
    }

    // 创建编组信息
    GroupInfo newGroup;
    newGroup.groupName = groupName;

    // 创建设备类型信息
    EquipmentType newEquipment;
    newEquipment.equipmentName = equipmentType;

    // 根据初始配置数量创建设备列表
    for (int i = 0; i < initialCount; ++i) {
        UAVInfo uav;
        // 生成设备名称，格式如 "UAV-1", "UAV-2"
        uav.uavName = equipmentType.split(" ").at(0) + QString("-%1").arg(i + 1);
        uav.status = "待命";
        newEquipment.uavList.append(uav);
    }

    newGroup.equipmentList.append(newEquipment);

    // 发送兵力添加信号并关闭对话框
    emit forceAdded(newGroup);
    accept();
}

/**
 * @brief 取消按钮处理
 * @details 关闭对话框，不保存数据
 */
void AddForceDialog::onCancelButtonClicked()
{
    reject();
}

/**
 * @brief 关闭按钮处理
 * @details 关闭对话框
 */
void AddForceDialog::onCloseButtonClicked()
{
    reject();
}
