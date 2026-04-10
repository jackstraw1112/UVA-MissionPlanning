#include "TargetParamDialog.h"
#include "ui_TargetParamDialog.h"
#include <QMessageBox>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 * @details 初始化成员变量，设置UI连接信号槽
 */
TargetParamDialog::TargetParamDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TargetParamDialog)
    , m_editIndex(-1)
    , m_isEditMode(false)
    , m_planName("")
    , m_coordinationName("")
{
    ui->setupUi(this);

    // 连接目标类型切换信号
    connect(ui->targetTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TargetParamDialog::onTargetTypeChanged);
    // 连接按钮信号
    connect(ui->okButton, &QPushButton::clicked, this, &TargetParamDialog::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TargetParamDialog::onCancelButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &TargetParamDialog::onCloseButtonClicked);

    // 默认显示雷达目标参数页面
    ui->paramStackedWidget->setCurrentIndex(0);
}

/**
 * @brief 析构函数
 * @details 释放UI资源
 */
TargetParamDialog::~TargetParamDialog()
{
    delete ui;
}

/**
 * @brief 设置方案信息
 * @param planName 方案名称
 * @param coordinationName 协同规划名称
 */
void TargetParamDialog::setPlanInfo(const QString& planName, const QString& coordinationName)
{
    m_planName = planName;
    m_coordinationName = coordinationName;
}

/**
 * @brief 设置雷达目标参数
 * @param radar 雷达目标参数
 * @details 用于编辑模式，将雷达目标参数填充到表单
 */
void TargetParamDialog::setRadarTarget(const RadarTargetParam& radar)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = radar.planName;
    m_coordinationName = radar.coordinationName;

    // 切换到雷达目标页面
    ui->targetTypeComboBox->setCurrentIndex(0);

    // 填充雷达目标参数
    ui->targetIdEdit->setText(radar.targetId);

    // 解析频率范围
    QStringList freqParts = radar.frequencyRange.split(" ~ ");
    if (freqParts.size() == 2) {
        ui->radarFreqMinEdit->setText(freqParts[0].trimmed());
        ui->radarFreqMaxEdit->setText(freqParts[1].trimmed());
    }

    // 解析脉冲宽度范围
    QStringList pulseParts = radar.pulseWidthRange.split(" ~ ");
    if (pulseParts.size() == 2) {
        ui->radarPulseWidthMinEdit->setText(pulseParts[0].trimmed());
        ui->radarPulseWidthMaxEdit->setText(pulseParts[1].trimmed());
    }

    // 解析重复周期范围
    QStringList repParts = radar.repetitionPeriod.split(" ~ ");
    if (repParts.size() == 2) {
        ui->radarRepPeriodMinEdit->setText(repParts[0].trimmed());
        ui->radarRepPeriodMaxEdit->setText(repParts[1].trimmed());
    }

    // 设置工作模式
    int modeIndex = ui->radarWorkingModeComboBox->findText(radar.workingMode);
    if (modeIndex >= 0) {
        ui->radarWorkingModeComboBox->setCurrentIndex(modeIndex);
    }
}

/**
 * @brief 设置无线电目标参数
 * @param radio 无线电目标参数
 * @details 用于编辑模式，将无线电目标参数填充到表单
 */
void TargetParamDialog::setRadioTarget(const RadioTargetParam& radio)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = radio.planName;
    m_coordinationName = radio.coordinationName;

    // 切换到无线电目标页面
    ui->targetTypeComboBox->setCurrentIndex(1);

    // 填充无线电目标参数
    ui->targetIdEdit->setText(radio.targetId);

    // 解析频率范围
    QStringList freqParts = radio.frequencyRange.split(" ~ ");
    if (freqParts.size() == 2) {
        ui->radioFreqMinEdit->setText(freqParts[0].trimmed());
        ui->radioFreqMaxEdit->setText(freqParts[1].trimmed());
    }

    // 设置调制模式
    int modIndex = ui->radioModulationModeComboBox->findText(radio.modulationMode);
    if (modIndex >= 0) {
        ui->radioModulationModeComboBox->setCurrentIndex(modIndex);
    }

    ui->radioSignalBandwidthEdit->setText(radio.signalBandwidth);
    ui->radioTransmitPowerEdit->setText(radio.transmitPower);
}

/**
 * @brief 设置通信干扰参数
 * @param comm 通信干扰参数
 * @details 用于编辑模式，将通信干扰参数填充到表单
 */
void TargetParamDialog::setCommJamming(const CommJammingParam& comm)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = comm.planName;
    m_coordinationName = comm.coordinationName;

    // 切换到通信干扰页面
    ui->targetTypeComboBox->setCurrentIndex(2);

    // 填充通信干扰参数
    ui->targetIdEdit->setText(comm.targetId);
    ui->commFreqEdit->setText(comm.jammingFrequency);

    // 设置干扰模式
    int modeIndex = ui->commModeComboBox->findText(comm.jammingMode);
    if (modeIndex >= 0) {
        ui->commModeComboBox->setCurrentIndex(modeIndex);
    }

    ui->commPowerEdit->setText(comm.jammingPower);
    ui->commCoverageEdit->setText(comm.coverageRange);
}

/**
 * @brief 设置雷达干扰参数
 * @param rcm 雷达干扰参数
 * @details 用于编辑模式，将雷达干扰参数填充到表单
 */
void TargetParamDialog::setRcmJamming(const RcmJammingParam& rcm)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = rcm.planName;
    m_coordinationName = rcm.coordinationName;

    // 切换到雷达干扰页面
    ui->targetTypeComboBox->setCurrentIndex(3);

    // 填充雷达干扰参数
    ui->targetIdEdit->setText(rcm.targetId);
    ui->rcmFreqEdit->setText(rcm.jammingFrequency);

    // 设置干扰模式
    int modeIndex = ui->rcmModeComboBox->findText(rcm.jammingMode);
    if (modeIndex >= 0) {
        ui->rcmModeComboBox->setCurrentIndex(modeIndex);
    }

    ui->rcmPowerEdit->setText(rcm.jammingPower);
    ui->rcmCoverageEdit->setText(rcm.coverageRange);
}

/**
 * @brief 目标类型切换处理
 * @param index 切换到的索引
 * @details 根据选择的索引切换参数页面显示
 */
void TargetParamDialog::onTargetTypeChanged(int index)
{
    ui->paramStackedWidget->setCurrentIndex(index);
}

/**
 * @brief 确定按钮处理
 * @details 验证输入，根据目标类型收集参数，发送相应信号
 */
void TargetParamDialog::onOkButtonClicked()
{
    QString targetId = ui->targetIdEdit->text().trimmed();
    if (targetId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入目标编号");
        return;
    }

    int targetType = ui->targetTypeComboBox->currentIndex();

    switch (targetType) {
    case 0: {
        // 雷达目标
        RadarTargetParam radar;
        radar.planName = m_planName;
        radar.coordinationName = m_coordinationName;
        radar.targetId = targetId;
        radar.frequencyRange = QString("%1 ~ %2").arg(ui->radarFreqMinEdit->text()).arg(ui->radarFreqMaxEdit->text());
        radar.pulseWidthRange = QString("%1 ~ %2").arg(ui->radarPulseWidthMinEdit->text()).arg(ui->radarPulseWidthMaxEdit->text());
        radar.repetitionPeriod = QString("%1 ~ %2").arg(ui->radarRepPeriodMinEdit->text()).arg(ui->radarRepPeriodMaxEdit->text());
        radar.workingMode = ui->radarWorkingModeComboBox->currentText();
        if (m_isEditMode) {
            emit radarTargetUpdated(radar, m_editIndex);
        } else {
            emit radarTargetAdded(radar);
        }
        break;
    }
    case 1: {
        // 无线电目标
        RadioTargetParam radio;
        radio.planName = m_planName;
        radio.coordinationName = m_coordinationName;
        radio.targetId = targetId;
        radio.frequencyRange = QString("%1 ~ %2").arg(ui->radioFreqMinEdit->text()).arg(ui->radioFreqMaxEdit->text());
        radio.modulationMode = ui->radioModulationModeComboBox->currentText();
        radio.signalBandwidth = ui->radioSignalBandwidthEdit->text();
        radio.transmitPower = ui->radioTransmitPowerEdit->text();
        if (m_isEditMode) {
            emit radioTargetUpdated(radio, m_editIndex);
        } else {
            emit radioTargetAdded(radio);
        }
        break;
    }
    case 2: {
        // 通信干扰
        CommJammingParam comm;
        comm.planName = m_planName;
        comm.coordinationName = m_coordinationName;
        comm.targetId = targetId;
        comm.jammingFrequency = ui->commFreqEdit->text();
        comm.jammingMode = ui->commModeComboBox->currentText();
        comm.jammingPower = ui->commPowerEdit->text();
        comm.coverageRange = ui->commCoverageEdit->text();
        if (m_isEditMode) {
            emit commJammingUpdated(comm, m_editIndex);
        } else {
            emit commJammingAdded(comm);
        }
        break;
    }
    case 3: {
        // 雷达干扰
        RcmJammingParam rcm;
        rcm.planName = m_planName;
        rcm.coordinationName = m_coordinationName;
        rcm.targetId = targetId;
        rcm.jammingFrequency = ui->rcmFreqEdit->text();
        rcm.jammingMode = ui->rcmModeComboBox->currentText();
        rcm.jammingPower = ui->rcmPowerEdit->text();
        rcm.coverageRange = ui->rcmCoverageEdit->text();
        if (m_isEditMode) {
            emit rcmJammingUpdated(rcm, m_editIndex);
        } else {
            emit rcmJammingAdded(rcm);
        }
        break;
    }
    }

    accept();
}

/**
 * @brief 取消按钮处理
 * @details 关闭对话框，不保存数据
 */
void TargetParamDialog::onCancelButtonClicked()
{
    reject();
}

/**
 * @brief 关闭按钮处理
 * @details 关闭对话框
 */
void TargetParamDialog::onCloseButtonClicked()
{
    reject();
}
