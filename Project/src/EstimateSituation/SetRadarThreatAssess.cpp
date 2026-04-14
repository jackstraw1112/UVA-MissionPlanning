//
// Created by admin on "2026.04.13 T 11:17:28".
//

#include "SetRadarThreatAssess.h"
#include "ui_SetRadarThreatAssess.h"

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QtMath>

void SetRadarThreatAssess::connectPanelWidgetRelays()
{
    connect(ui->btnCalculate, &QPushButton::clicked, this, &SetRadarThreatAssess::onCalculate);
    connect(ui->btnConfirm, &QPushButton::clicked, this, &SetRadarThreatAssess::onConfirm);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SetRadarThreatAssess::onCancel);
}

void SetRadarThreatAssess::onCalculate()
{
    const RadarPerformancePara uiPerformance = readInputFromUi();
    QString validationError;
    if (!ProjectPublicInterface::validateRadarInput(uiPerformance, &validationError))
    {
        RadarThreatAssessResult invalidResult;
        invalidResult.errorMessage = validationError;
        QMessageBox::warning(this, QStringLiteral("参数错误"), validationError);
        displayResult(invalidResult);
        return;
    }

    const RadarThreatFactors synthesis = readSubfactorSynthesisFromUi();
    const RadarTypicalPara representativeCenters = readRepresentativeCentersFromUi();
    const RadarThreatAssessResult assessmentResult =
            ProjectPublicInterface::calculateThreatResult(uiPerformance, representativeCenters, synthesis);
    if (!assessmentResult.valid)
    {
        RadarThreatAssessResult invalidResult;
        invalidResult.errorMessage = assessmentResult.errorMessage;
        QMessageBox::warning(this, QStringLiteral("参数错误"), assessmentResult.errorMessage);
        displayResult(invalidResult);
        return;
    }

    displayResult(assessmentResult);
}

void SetRadarThreatAssess::onConfirm()
{
    // 点击确定时统一计算并提交当前页结果到表格侧
    onCalculate();
    emit sigEvaluteResult();
}

void SetRadarThreatAssess::onCancel()
{
    this->hide();
}

RadarPerformancePara SetRadarThreatAssess::readInputFromUi() const
{
    RadarPerformancePara performance;
    performance.freqMin = ui->spinFreqMin->value();
    performance.freqMax = ui->spinFreqMax->value();
    performance.pwMin = ui->spinPwMin->value();
    performance.pwMax = ui->spinPwMax->value();
    performance.prfMin = ui->spinPrfMin->value();
    performance.prfMax = ui->spinPrfMax->value();
    performance.detectRange = ui->spinRangeKm->value();
    return performance;
}

RadarTypicalPara SetRadarThreatAssess::readRepresentativeCentersFromUi() const
{
    RadarTypicalPara representativeCenters;
    representativeCenters.freq = ui->spinResultFreqCenter->value();
    representativeCenters.pw = ui->spinResultPwCenter->value();
    representativeCenters.prf = ui->spinResultPrfCenter->value();
    representativeCenters.valid = true;
    return representativeCenters;
}

RadarThreatFactors SetRadarThreatAssess::readSubfactorSynthesisFromUi() const
{
    RadarThreatFactors factors;
    factors.freqFactor = ui->spinResultFFreq->value();
    factors.prfFactor = ui->spinResultFPrf->value();
    factors.pwFactor = ui->spinResultFPw->value();
    factors.f1RawFactor = ui->spinResultF1Raw->value();
    factors.rangeModFactor = ui->spinResultRangeMod->value();
    factors.f1Factor = ui->spinResultF1->value();
    return factors;
}

void SetRadarThreatAssess::displaySubfactor(const RadarThreatFactors &factors)
{
    m_blockResultSpinSignals = true;
    ui->spinResultFFreq->setValue(factors.freqFactor);
    ui->spinResultFPrf->setValue(factors.prfFactor);
    ui->spinResultFPw->setValue(factors.pwFactor);
    ui->spinResultF1Raw->setValue(factors.f1RawFactor);
    ui->spinResultRangeMod->setValue(factors.rangeModFactor);
    ui->spinResultF1->setValue(factors.f1Factor);
    m_blockResultSpinSignals = false;
}

void SetRadarThreatAssess::syncRepresentativeCentersToUi(const RadarTypicalPara &representative,
                                                   const RadarPerformancePara &performance,
                                                   const RadarThreatAssessResult &fallbackResult)
{
    m_blockResultSpinSignals = true;
    if (representative.valid)
    {
        ui->spinResultFreqCenter->setValue(representative.freq);
        ui->spinResultPwCenter->setValue(representative.pw);
        ui->spinResultPrfCenter->setValue(representative.prf);
    }
    else if (fallbackResult.valid)
    {
        ui->spinResultFreqCenter->setValue(fallbackResult.freq);
        ui->spinResultPwCenter->setValue(fallbackResult.pw);
        ui->spinResultPrfCenter->setValue(fallbackResult.prf);
    }
    else if (ProjectPublicInterface::validateRadarInput(performance, nullptr))
    {
        ui->spinResultFreqCenter->setValue(ProjectPublicInterface::geometricMean(performance.freqMin, performance.freqMax));
        ui->spinResultPwCenter->setValue(
                ProjectPublicInterface::geometricMean(performance.pwMin, performance.pwMax));
        ui->spinResultPrfCenter->setValue(ProjectPublicInterface::geometricMean(performance.prfMin, performance.prfMax));
    }
    else
    {
        ui->spinResultFreqCenter->setValue(0.0);
        ui->spinResultPwCenter->setValue(0.0);
        ui->spinResultPrfCenter->setValue(0.0);
    }
    m_blockResultSpinSignals = false;
}

void SetRadarThreatAssess::displayResult(const RadarThreatAssessResult &assessment)
{
    if (!assessment.valid)
    {
        ui->lblSituationSummary->clear();
        ui->progressF1->setValue(0);
        ui->lblEffectiveVal->setText(QStringLiteral("—"));
        return;
    }

    QString radiationPowerText;
    QString ratedPowerText;
    if (ui->spinRadPowerKw->value() > 0.0)
    {
        radiationPowerText = QString::number(ui->spinRadPowerKw->value(), 'f', 2);
    }
    else
    {
        radiationPowerText = QStringLiteral("未填");
    }
    if (ui->spinRatedPowerKw->value() > 0.0)
    {
        ratedPowerText = QString::number(ui->spinRatedPowerKw->value(), 'f', 2);
    }
    else
    {
        ratedPowerText = QStringLiteral("未填");
    }
    ui->lblSituationSummary->setText(
            QStringLiteral("阵地火力防护：%1 | 当前辐射功率：%2 kW | 额定功率：%3 kW（记录项；未录入时批量计算按默认态势：中防护、正常辐射模式）")
            .arg(ui->comboPositionDefense->currentText(), radiationPowerText, ratedPowerText));

    const double f1Value = ui->spinResultF1->value();
    const int f1Percent = static_cast<int>(qRound(f1Value * 100.0));
    ui->progressF1->setValue(qBound(0, f1Percent, 100));

    int radiationModeIndex = ui->comboRadiationMode->currentIndex();
    if (radiationModeIndex < 0)
    {
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationModeComboIndex;
    }
    const double radiationMultiplier = ProjectPublicInterface::radiationModeMultiplier(radiationModeIndex);
    const double effectiveThreat = ProjectPublicInterface::clamp01(f1Value * radiationMultiplier);
    ui->lblEffectiveVal->setText(QString::number(effectiveThreat, 'f', 4));
}

void SetRadarThreatAssess::refreshSituationSummaryEffectiveProgress(RadarThreatAssessResult *lastResult)
{
    QString radiationPowerText;
    QString ratedPowerText;
    if (ui->spinRadPowerKw->value() > 0.0)
    {
        radiationPowerText = QString::number(ui->spinRadPowerKw->value(), 'f', 2);
    }
    else
    {
        radiationPowerText = QStringLiteral("未填");
    }
    if (ui->spinRatedPowerKw->value() > 0.0)
    {
        ratedPowerText = QString::number(ui->spinRatedPowerKw->value(), 'f', 2);
    }
    else
    {
        ratedPowerText = QStringLiteral("未填");
    }
    ui->lblSituationSummary->setText(
            QStringLiteral("阵地火力防护：%1 | 当前辐射功率：%2 kW | 额定功率：%3 kW（记录项；未录入时批量计算按默认态势：中防护、正常辐射模式）")
            .arg(ui->comboPositionDefense->currentText(), radiationPowerText, ratedPowerText));

    const double f1FromSpin = ui->spinResultF1->value();
    int radiationModeIndex = ui->comboRadiationMode->currentIndex();
    if (radiationModeIndex < 0)
    {
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationModeComboIndex;
    }
    const double radiationMultiplier = ProjectPublicInterface::radiationModeMultiplier(radiationModeIndex);
    ui->lblEffectiveVal->setText(QString::number(ProjectPublicInterface::clamp01(f1FromSpin * radiationMultiplier), 'f', 4));
    ui->progressF1->setValue(qBound(0, static_cast<int>(qRound(f1FromSpin * 100.0)), 100));

    Q_UNUSED(lastResult);
}

void SetRadarThreatAssess::clearTargetEdits()
{
    ui->editRadarModel->clear();
    ui->editEquipmentEntity->clear();
}

void SetRadarThreatAssess::clearEquipmentEntityEdit()
{
    ui->editEquipmentEntity->clear();
}

void SetRadarThreatAssess::setEditRadarModel(const QString &text)
{
    ui->editRadarModel->setText(text);
}

void SetRadarThreatAssess::setEditEquipmentEntity(const QString &text)
{
    ui->editEquipmentEntity->setText(text);
}

void SetRadarThreatAssess::setFreqMin(double v)
{
    ui->spinFreqMin->setValue(v);
}

void SetRadarThreatAssess::setFreqMax(double v)
{
    ui->spinFreqMax->setValue(v);
}

void SetRadarThreatAssess::setPwMin(double v)
{
    ui->spinPwMin->setValue(v);
}

void SetRadarThreatAssess::setPwMax(double v)
{
    ui->spinPwMax->setValue(v);
}

void SetRadarThreatAssess::setPrfMin(double v)
{
    ui->spinPrfMin->setValue(v);
}

void SetRadarThreatAssess::setPrfMax(double v)
{
    ui->spinPrfMax->setValue(v);
}

void SetRadarThreatAssess::setRangeKm(double v)
{
    ui->spinRangeKm->setValue(v);
}

void SetRadarThreatAssess::setDefenseComboIndex(int index)
{
    ui->comboPositionDefense->setCurrentIndex(index);
}

void SetRadarThreatAssess::setRadiationComboIndex(int index)
{
    ui->comboRadiationMode->setCurrentIndex(index);
}

int SetRadarThreatAssess::defenseComboCurrentIndex() const
{
    return ui->comboPositionDefense->currentIndex();
}

int SetRadarThreatAssess::radiationComboCurrentIndex() const
{
    return ui->comboRadiationMode->currentIndex();
}

double SetRadarThreatAssess::currentResultF1SpinValue() const
{
    return ui->spinResultF1->value();
}

SetRadarThreatAssess::SetRadarThreatAssess(QWidget *parent)
    : QWidget(parent), ui(new Ui::SetRadarThreatAssess)
{
    ui->setupUi(this);
    connectPanelWidgetRelays();
}

SetRadarThreatAssess::~SetRadarThreatAssess()
{
    delete ui;
}
