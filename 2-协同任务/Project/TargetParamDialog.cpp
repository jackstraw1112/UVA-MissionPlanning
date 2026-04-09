#include "TargetParamDialog.h"
#include "ui_TargetParamDialog.h"
#include <QMessageBox>

TargetParamDialog::TargetParamDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TargetParamDialog)
    , m_editIndex(-1)
    , m_isEditMode(false)
    , m_planName("")
    , m_coordinationName("")
{
    ui->setupUi(this);

    connect(ui->targetTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TargetParamDialog::onTargetTypeChanged);
    connect(ui->okButton, &QPushButton::clicked, this, &TargetParamDialog::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TargetParamDialog::onCancelButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &TargetParamDialog::onCloseButtonClicked);

    ui->paramStackedWidget->setCurrentIndex(0);
}

TargetParamDialog::~TargetParamDialog()
{
    delete ui;
}

void TargetParamDialog::setPlanInfo(const QString& planName, const QString& coordinationName)
{
    m_planName = planName;
    m_coordinationName = coordinationName;
}

void TargetParamDialog::setRadarTarget(const RadarTargetParam& radar)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = radar.planName;
    m_coordinationName = radar.coordinationName;
    ui->targetTypeComboBox->setCurrentIndex(0);
    ui->targetIdEdit->setText(radar.targetId);
    QStringList freqParts = radar.frequencyRange.split(" ~ ");
    if (freqParts.size() == 2) {
        ui->radarFreqMinEdit->setText(freqParts[0].trimmed());
        ui->radarFreqMaxEdit->setText(freqParts[1].trimmed());
    }
    QStringList pulseParts = radar.pulseWidthRange.split(" ~ ");
    if (pulseParts.size() == 2) {
        ui->radarPulseWidthMinEdit->setText(pulseParts[0].trimmed());
        ui->radarPulseWidthMaxEdit->setText(pulseParts[1].trimmed());
    }
    QStringList repParts = radar.repetitionPeriod.split(" ~ ");
    if (repParts.size() == 2) {
        ui->radarRepPeriodMinEdit->setText(repParts[0].trimmed());
        ui->radarRepPeriodMaxEdit->setText(repParts[1].trimmed());
    }
    int modeIndex = ui->radarWorkingModeComboBox->findText(radar.workingMode);
    if (modeIndex >= 0) {
        ui->radarWorkingModeComboBox->setCurrentIndex(modeIndex);
    }
}

void TargetParamDialog::setRadioTarget(const RadioTargetParam& radio)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = radio.planName;
    m_coordinationName = radio.coordinationName;
    ui->targetTypeComboBox->setCurrentIndex(1);
    ui->targetIdEdit->setText(radio.targetId);
    QStringList freqParts = radio.frequencyRange.split(" ~ ");
    if (freqParts.size() == 2) {
        ui->radioFreqMinEdit->setText(freqParts[0].trimmed());
        ui->radioFreqMaxEdit->setText(freqParts[1].trimmed());
    }
    int modIndex = ui->radioModulationModeComboBox->findText(radio.modulationMode);
    if (modIndex >= 0) {
        ui->radioModulationModeComboBox->setCurrentIndex(modIndex);
    }
    ui->radioSignalBandwidthEdit->setText(radio.signalBandwidth);
    ui->radioTransmitPowerEdit->setText(radio.transmitPower);
}

void TargetParamDialog::setCommJamming(const CommJammingParam& comm)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = comm.planName;
    m_coordinationName = comm.coordinationName;
    ui->targetTypeComboBox->setCurrentIndex(2);
    ui->targetIdEdit->setText(comm.targetId);
    ui->commFreqEdit->setText(comm.jammingFrequency);
    int modeIndex = ui->commModeComboBox->findText(comm.jammingMode);
    if (modeIndex >= 0) {
        ui->commModeComboBox->setCurrentIndex(modeIndex);
    }
    ui->commPowerEdit->setText(comm.jammingPower);
    ui->commCoverageEdit->setText(comm.coverageRange);
}

void TargetParamDialog::setRcmJamming(const RcmJammingParam& rcm)
{
    m_isEditMode = true;
    m_editIndex = -1;
    m_planName = rcm.planName;
    m_coordinationName = rcm.coordinationName;
    ui->targetTypeComboBox->setCurrentIndex(3);
    ui->targetIdEdit->setText(rcm.targetId);
    ui->rcmFreqEdit->setText(rcm.jammingFrequency);
    int modeIndex = ui->rcmModeComboBox->findText(rcm.jammingMode);
    if (modeIndex >= 0) {
        ui->rcmModeComboBox->setCurrentIndex(modeIndex);
    }
    ui->rcmPowerEdit->setText(rcm.jammingPower);
    ui->rcmCoverageEdit->setText(rcm.coverageRange);
}

void TargetParamDialog::onTargetTypeChanged(int index)
{
    ui->paramStackedWidget->setCurrentIndex(index);
}

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

void TargetParamDialog::onCancelButtonClicked()
{
    reject();
}

void TargetParamDialog::onCloseButtonClicked()
{
    reject();
}
