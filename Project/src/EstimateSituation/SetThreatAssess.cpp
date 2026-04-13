//
// Created by admin on "2026.04.13 T 11:17:28".
//

#include "SetThreatAssess.h"
#include "ui_SetThreatAssess.h"
#include "ui_RZThreatAssess.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QtMath>

namespace ThreatAssessUi
{
    double radiationModeMultiplier(int comboIndex)
    {
        switch (comboIndex)
        {
            case 0:
                return 1.0;
            case 1:
                return 1.0;
            case 2:
                return 0.85;
            case 3:
                return 0.75;
            case 4:
                return 0.0;
            default:
                return 1.0;
        }
    }

    double clamp01(double value)
    {
        if (value < 0.0)
        {
            return 0.0;
        }
        if (value > 1.0)
        {
            return 1.0;
        }
        return value;
    }
} // namespace ThreatAssessUi

void ThreatAssessFormBridge::attachToRZUi(Ui::RZThreatAssess *ui)
{
    m_editRadarModel = ui->editRadarModel;
    m_editEquipmentEntity = ui->editEquipmentEntity;
    m_comboPositionDefense = ui->comboPositionDefense;
    m_comboRadiationMode = ui->comboRadiationMode;
    m_spinRadPowerKw = ui->spinRadPowerKw;
    m_spinRatedPowerKw = ui->spinRatedPowerKw;
    m_spinFreqMin = ui->spinFreqMin;
    m_spinFreqMax = ui->spinFreqMax;
    m_spinPwMin = ui->spinPwMin;
    m_spinPwMax = ui->spinPwMax;
    m_spinPrfMin = ui->spinPrfMin;
    m_spinPrfMax = ui->spinPrfMax;
    m_spinRangeKm = ui->spinRangeKm;
    m_btnCalculate = ui->btnCalculate;
    m_spinResultFreqCenter = ui->spinResultFreqCenter;
    m_spinResultPwCenter = ui->spinResultPwCenter;
    m_spinResultPrfCenter = ui->spinResultPrfCenter;
    m_spinResultFFreq = ui->spinResultFFreq;
    m_spinResultFPrf = ui->spinResultFPrf;
    m_spinResultFPw = ui->spinResultFPw;
    m_spinResultF1Raw = ui->spinResultF1Raw;
    m_spinResultRangeMod = ui->spinResultRangeMod;
    m_spinResultF1 = ui->spinResultF1;
    m_lblSituationSummary = ui->lblSituationSummary;
    m_lblEffectiveVal = ui->lblEffectiveVal;
    m_progressF1 = ui->progressF1;
}

void ThreatAssessFormBridge::attachToSetUi(Ui::SetThreatAssess *ui)
{
    m_editRadarModel = ui->editRadarModel;
    m_editEquipmentEntity = ui->editEquipmentEntity;
    m_comboPositionDefense = ui->comboPositionDefense;
    m_comboRadiationMode = ui->comboRadiationMode;
    m_spinRadPowerKw = ui->spinRadPowerKw;
    m_spinRatedPowerKw = ui->spinRatedPowerKw;
    m_spinFreqMin = ui->spinFreqMin;
    m_spinFreqMax = ui->spinFreqMax;
    m_spinPwMin = ui->spinPwMin;
    m_spinPwMax = ui->spinPwMax;
    m_spinPrfMin = ui->spinPrfMin;
    m_spinPrfMax = ui->spinPrfMax;
    m_spinRangeKm = ui->spinRangeKm;
    m_btnCalculate = ui->btnCalculate;
    m_spinResultFreqCenter = ui->spinResultFreqCenter;
    m_spinResultPwCenter = ui->spinResultPwCenter;
    m_spinResultPrfCenter = ui->spinResultPrfCenter;
    m_spinResultFFreq = ui->spinResultFFreq;
    m_spinResultFPrf = ui->spinResultFPrf;
    m_spinResultFPw = ui->spinResultFPw;
    m_spinResultF1Raw = ui->spinResultF1Raw;
    m_spinResultRangeMod = ui->spinResultRangeMod;
    m_spinResultF1 = ui->spinResultF1;
    m_lblSituationSummary = ui->lblSituationSummary;
    m_lblEffectiveVal = ui->lblEffectiveVal;
    m_progressF1 = ui->progressF1;
}

RadarPerformancePara ThreatAssessFormBridge::readInputFromUi() const
{
    RadarPerformancePara performance;
    performance.freqMinGhz = m_spinFreqMin->value();
    performance.freqMaxGhz = m_spinFreqMax->value();
    performance.pulseWidthMinUs = m_spinPwMin->value();
    performance.pulseWidthMaxUs = m_spinPwMax->value();
    performance.prfMinHz = m_spinPrfMin->value();
    performance.prfMaxHz = m_spinPrfMax->value();
    performance.detectRangeKm = m_spinRangeKm->value();
    return performance;
}

RadarTypicalPara ThreatAssessFormBridge::readRepresentativeCentersFromUi() const
{
    RadarTypicalPara representativeCenters;
    representativeCenters.freqCenterGhz = m_spinResultFreqCenter->value();
    representativeCenters.pulseWidthCenterUs = m_spinResultPwCenter->value();
    representativeCenters.prfCenterHz = m_spinResultPrfCenter->value();
    representativeCenters.valid = true;
    return representativeCenters;
}

GlobalThreatFactors ThreatAssessFormBridge::readSubfactorSynthesisFromUi() const
{
    GlobalThreatFactors factors;
    factors.fFreq = m_spinResultFFreq->value();
    factors.fPrf = m_spinResultFPrf->value();
    factors.fPw = m_spinResultFPw->value();
    factors.f1Raw = m_spinResultF1Raw->value();
    factors.rangeMod = m_spinResultRangeMod->value();
    factors.f1 = m_spinResultF1->value();
    return factors;
}

void ThreatAssessFormBridge::syncSubfactorSpinBoxes(const GlobalThreatFactors &factors)
{
    m_blockResultSpinSignals = true;
    m_spinResultFFreq->setValue(factors.fFreq);
    m_spinResultFPrf->setValue(factors.fPrf);
    m_spinResultFPw->setValue(factors.fPw);
    m_spinResultF1Raw->setValue(factors.f1Raw);
    m_spinResultRangeMod->setValue(factors.rangeMod);
    m_spinResultF1->setValue(factors.f1);
    m_blockResultSpinSignals = false;
}

void ThreatAssessFormBridge::syncRepresentativeCentersToUi(const RadarTypicalPara &representative,
                                                           const RadarPerformancePara &performance,
                                                           const RadarThreatAssessResult &fallbackResult)
{
    m_blockResultSpinSignals = true;
    if (representative.valid)
    {
        m_spinResultFreqCenter->setValue(representative.freqCenterGhz);
        m_spinResultPwCenter->setValue(representative.pulseWidthCenterUs);
        m_spinResultPrfCenter->setValue(representative.prfCenterHz);
    }
    else if (fallbackResult.valid)
    {
        m_spinResultFreqCenter->setValue(fallbackResult.freqCenterGhz);
        m_spinResultPwCenter->setValue(fallbackResult.pulseWidthCenterUs);
        m_spinResultPrfCenter->setValue(fallbackResult.prfCenterHz);
    }
    else if (RZThreatAssess::validateRadarInput(performance, nullptr))
    {
        m_spinResultFreqCenter->setValue(RZThreatAssess::geometricMean(performance.freqMinGhz, performance.freqMaxGhz));
        m_spinResultPwCenter->setValue(
                RZThreatAssess::geometricMean(performance.pulseWidthMinUs, performance.pulseWidthMaxUs));
        m_spinResultPrfCenter->setValue(RZThreatAssess::geometricMean(performance.prfMinHz, performance.prfMaxHz));
    }
    else
    {
        m_spinResultFreqCenter->setValue(0.0);
        m_spinResultPwCenter->setValue(0.0);
        m_spinResultPrfCenter->setValue(0.0);
    }
    m_blockResultSpinSignals = false;
}

void ThreatAssessFormBridge::displayResult(const RadarThreatAssessResult &assessment)
{
    if (!assessment.valid)
    {
        m_lblSituationSummary->clear();
        m_progressF1->setValue(0);
        m_lblEffectiveVal->setText(QStringLiteral("—"));
        return;
    }

    QString radiationPowerText;
    QString ratedPowerText;
    if (m_spinRadPowerKw->value() > 0.0)
    {
        radiationPowerText = QString::number(m_spinRadPowerKw->value(), 'f', 2);
    }
    else
    {
        radiationPowerText = QStringLiteral("未填");
    }
    if (m_spinRatedPowerKw->value() > 0.0)
    {
        ratedPowerText = QString::number(m_spinRatedPowerKw->value(), 'f', 2);
    }
    else
    {
        ratedPowerText = QStringLiteral("未填");
    }
    m_lblSituationSummary->setText(
            QStringLiteral("阵地火力防护：%1 | 当前辐射功率：%2 kW | 额定功率：%3 kW（记录项；未录入时批量计算按默认态势：中防护、正常辐射模式）")
            .arg(m_comboPositionDefense->currentText(), radiationPowerText, ratedPowerText));

    const int f1Percent = static_cast<int>(qRound(assessment.f1 * 100.0));
    m_progressF1->setValue(qBound(0, f1Percent, 100));

    int radiationModeIndex = m_comboRadiationMode->currentIndex();
    if (radiationModeIndex < 0)
    {
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationModeComboIndex;
    }
    const double radiationMultiplier = ThreatAssessUi::radiationModeMultiplier(radiationModeIndex);
    const double effectiveThreat = ThreatAssessUi::clamp01(assessment.f1 * radiationMultiplier);
    m_lblEffectiveVal->setText(QString::number(effectiveThreat, 'f', 4));
}

void ThreatAssessFormBridge::refreshSituationSummaryEffectiveProgress(RadarThreatAssessResult *lastResult)
{
    QString radiationPowerText;
    QString ratedPowerText;
    if (m_spinRadPowerKw->value() > 0.0)
    {
        radiationPowerText = QString::number(m_spinRadPowerKw->value(), 'f', 2);
    }
    else
    {
        radiationPowerText = QStringLiteral("未填");
    }
    if (m_spinRatedPowerKw->value() > 0.0)
    {
        ratedPowerText = QString::number(m_spinRatedPowerKw->value(), 'f', 2);
    }
    else
    {
        ratedPowerText = QStringLiteral("未填");
    }
    m_lblSituationSummary->setText(
            QStringLiteral("阵地火力防护：%1 | 当前辐射功率：%2 kW | 额定功率：%3 kW（记录项；未录入时批量计算按默认态势：中防护、正常辐射模式）")
            .arg(m_comboPositionDefense->currentText(), radiationPowerText, ratedPowerText));

    const double f1FromSpin = m_spinResultF1->value();
    int radiationModeIndex = m_comboRadiationMode->currentIndex();
    if (radiationModeIndex < 0)
    {
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationModeComboIndex;
    }
    const double radiationMultiplier = ThreatAssessUi::radiationModeMultiplier(radiationModeIndex);
    m_lblEffectiveVal->setText(QString::number(ThreatAssessUi::clamp01(f1FromSpin * radiationMultiplier), 'f', 4));
    m_progressF1->setValue(qBound(0, static_cast<int>(qRound(f1FromSpin * 100.0)), 100));

    if (lastResult != nullptr && lastResult->valid)
    {
        lastResult->f1 = f1FromSpin;
    }
}

void ThreatAssessFormBridge::clearTargetEdits()
{
    m_editRadarModel->clear();
    m_editEquipmentEntity->clear();
}

void ThreatAssessFormBridge::clearEquipmentEntityEdit()
{
    m_editEquipmentEntity->clear();
}

void ThreatAssessFormBridge::setEditRadarModel(const QString &text)
{
    m_editRadarModel->setText(text);
}

void ThreatAssessFormBridge::setEditEquipmentEntity(const QString &text)
{
    m_editEquipmentEntity->setText(text);
}

void ThreatAssessFormBridge::setFreqMin(double v)
{
    m_spinFreqMin->setValue(v);
}

void ThreatAssessFormBridge::setFreqMax(double v)
{
    m_spinFreqMax->setValue(v);
}

void ThreatAssessFormBridge::setPwMin(double v)
{
    m_spinPwMin->setValue(v);
}

void ThreatAssessFormBridge::setPwMax(double v)
{
    m_spinPwMax->setValue(v);
}

void ThreatAssessFormBridge::setPrfMin(double v)
{
    m_spinPrfMin->setValue(v);
}

void ThreatAssessFormBridge::setPrfMax(double v)
{
    m_spinPrfMax->setValue(v);
}

void ThreatAssessFormBridge::setRangeKm(double v)
{
    m_spinRangeKm->setValue(v);
}

void ThreatAssessFormBridge::setDefenseComboIndex(int index)
{
    m_comboPositionDefense->setCurrentIndex(index);
}

void ThreatAssessFormBridge::setRadiationComboIndex(int index)
{
    m_comboRadiationMode->setCurrentIndex(index);
}

int ThreatAssessFormBridge::defenseComboCurrentIndex() const
{
    return m_comboPositionDefense->currentIndex();
}

int ThreatAssessFormBridge::radiationComboCurrentIndex() const
{
    return m_comboRadiationMode->currentIndex();
}

SetThreatAssess::SetThreatAssess(QWidget *parent)
    : QWidget(parent), ui(new Ui::SetThreatAssess)
{
    ui->setupUi(this);
    m_form.attachToSetUi(ui);
}

SetThreatAssess::~SetThreatAssess()
{
    delete ui;
}
