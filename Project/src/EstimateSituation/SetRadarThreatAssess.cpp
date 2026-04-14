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

SetRadarThreatAssess::SetRadarThreatAssess(QWidget *parent)
    : QWidget(parent), ui(new Ui::SetRadarThreatAssess)
{
    ui->setupUi(this);

    // 初始化参数
    initPara();

    // 初始化对象
    initClass();

    // 初始化连接
    signalAndSlot();
}

SetRadarThreatAssess::~SetRadarThreatAssess()
{
    delete ui;
}

void SetRadarThreatAssess::initPara()
{
}

void SetRadarThreatAssess::initClass()
{
}

void SetRadarThreatAssess::signalAndSlot()
{
    connect(ui->btnCalculate, &QPushButton::clicked, this, &SetRadarThreatAssess::onCalculate);
    connect(ui->btnConfirm, &QPushButton::clicked, this, &SetRadarThreatAssess::onConfirm);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SetRadarThreatAssess::onCancel);
}

void SetRadarThreatAssess::setDisplay(const RadarThreatAssessRecord &record)
{
    // 记录基本信息
    m_record = record;

    // 显示基本信息
    displayData(record, 1);

    // 显示实时参数
    displayData(record.workPara);

    // 显示典型参数
    displayData(record, 2);

    // 显示评估结果
    displayData(record.result);
}

void SetRadarThreatAssess::displayData(const RadarThreatAssessRecord &record, int flag)
{
    // 基本信息+显示性能参数
    if (flag == 1)
    {
        // 基本信息
        ui->editRadarModel->setText(record.typeName);
        ui->editEquipmentEntity->setText(record.entityName);

        // 显示性能参数
        const RadarPerformancePara &pref = record.perfPara;
        ui->spinFreqMin->setValue(pref.freqMin);
        ui->spinFreqMax->setValue(pref.freqMax);
        ui->spinPwMin->setValue(pref.pwMin);
        ui->spinPwMax->setValue(pref.pwMax);
        ui->spinPrfMin->setValue(pref.prfMin);
        ui->spinPrfMax->setValue(pref.prfMax);
        ui->spinRangeKm->setValue(pref.detectRange);
    }
    // 典型参数
    else
    {
        // 相关数据
        const RadarTypicalPara &typicalPara = record.typicalPara;
        const RadarPerformancePara &perf = record.perfPara;
        const RadarThreatAssessResult &assessResult = record.result;

        if (typicalPara.valid)
        {
            ui->spinResultFreqCenter->setValue(typicalPara.freq);
            ui->spinResultPwCenter->setValue(typicalPara.pw);
            ui->spinResultPrfCenter->setValue(typicalPara.prf);
        }
        else if (assessResult.valid)
        {
            ui->spinResultFreqCenter->setValue(assessResult.freq);
            ui->spinResultPwCenter->setValue(assessResult.pw);
            ui->spinResultPrfCenter->setValue(assessResult.prf);
        }
        else if (ProjectPublicInterface::validateRadarInput(perf, nullptr))
        {
            ui->spinResultFreqCenter->setValue(ProjectPublicInterface::geometricMean(perf.freqMin, perf.freqMax));
            ui->spinResultPwCenter->setValue(ProjectPublicInterface::geometricMean(perf.pwMin, perf.pwMax));
            ui->spinResultPrfCenter->setValue(ProjectPublicInterface::geometricMean(perf.prfMin, perf.prfMax));
        }
        else
        {
            ui->spinResultFreqCenter->setValue(0.0);
            ui->spinResultPwCenter->setValue(0.0);
            ui->spinResultPrfCenter->setValue(0.0);
        }
    }
}

void SetRadarThreatAssess::displayData(const RadarRealWrokPara &realPara)
{
    if (realPara.situationDefenseIndex >= 0)
    {
        ui->comboPositionDefense->setCurrentIndex(realPara.situationDefenseIndex);
    }
    else
    {
        ui->comboPositionDefense->setCurrentIndex(ThreatAssessUi::kDefaultPositionDefense);
    }

    if (realPara.situationRadModeIndex >= 0)
    {
        ui->comboRadiationMode->setCurrentIndex(realPara.situationRadModeIndex);
    }
    else
    {
        ui->comboRadiationMode->setCurrentIndex(ThreatAssessUi::kDefaultRadiationMode);
    }
}

void SetRadarThreatAssess::displayData(const RadarThreatFactors &factors)
{
    ui->spinResultFFreq->setValue(factors.freqFactor);
    ui->spinResultFPrf->setValue(factors.prfFactor);
    ui->spinResultFPw->setValue(factors.pwFactor);
    ui->spinResultF1Raw->setValue(factors.f1Raw);
    ui->spinResultRangeMod->setValue(factors.rangeMod);
    ui->spinResultF1->setValue(factors.F1);
}

void SetRadarThreatAssess::displayData(const RadarThreatAssessResult &assessment)
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
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationMode;
    }
    const double radiationMultiplier = ProjectPublicInterface::radiationModeMultiplier(radiationModeIndex);
    const double effectiveThreat = ProjectPublicInterface::clamp01(f1Value * radiationMultiplier);
    ui->lblEffectiveVal->setText(QString::number(effectiveThreat, 'f', 4));
}

void SetRadarThreatAssess::undisplayData()
{
    ui->editRadarModel->clear();
    ui->editEquipmentEntity->clear();
}

void SetRadarThreatAssess::onCalculate()
{
    // 判断参数是否有效
    QString validationError;
    if (!ProjectPublicInterface::validateRadarInput(m_record.perfPara, &validationError))
    {
        RadarThreatAssessResult invalidResult;
        invalidResult.errorMsg = validationError;
        QMessageBox::warning(this, QStringLiteral("参数错误"), validationError);
        displayData(invalidResult);
        return;
    }

    // 获取子因子
    const auto factor = readSubfactorFromUi();
    // 获取代表值
    const auto typicalPara = readTypicalFromUi();

    // 计算评估结果
    const auto result =  ProjectPublicInterface::calculateThreatResult(m_record);
    if (!m_record.result.valid)
    {
        RadarThreatAssessResult invalidResult;
        invalidResult.errorMsg = m_record.result.errorMsg;
        QMessageBox::warning(this, QStringLiteral("参数错误"), m_record.result.errorMsg);
        displayData(invalidResult);
        return;
    }

    // 缓存评估结果
    m_record.typicalPara = typicalPara;
    m_record.factors = factor;
    m_record.result = result;

    // 显示评估结果
    displayData(m_record.result);
}

void SetRadarThreatAssess::onConfirm()
{
    // 发送评估结果
    emit sigEvaluteResult();

    // 隐藏窗口
    this->hide();
}

void SetRadarThreatAssess::onCancel()
{
    this->hide();
}

RadarTypicalPara SetRadarThreatAssess::readTypicalFromUi() const
{
    RadarTypicalPara representativeCenters;
    representativeCenters.freq = ui->spinResultFreqCenter->value();
    representativeCenters.pw = ui->spinResultPwCenter->value();
    representativeCenters.prf = ui->spinResultPrfCenter->value();
    representativeCenters.valid = true;
    return representativeCenters;
}

RadarThreatFactors SetRadarThreatAssess::readSubfactorFromUi() const
{
    RadarThreatFactors factors;
    factors.freqFactor = ui->spinResultFFreq->value();
    factors.prfFactor = ui->spinResultFPrf->value();
    factors.pwFactor = ui->spinResultFPw->value();
    factors.f1Raw = ui->spinResultF1Raw->value();
    factors.rangeMod = ui->spinResultRangeMod->value();
    factors.F1 = ui->spinResultF1->value();
    return factors;
}