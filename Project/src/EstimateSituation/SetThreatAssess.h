//
// Created by admin on "2026.04.13 T 11:17:28".
//
// horizontalLayout_3 内控件的业务读写与态势展示（与 RZThreatAssess.ui / SetThreatAssess.ui 中同名控件对应）。
//

#ifndef SETTHREATASSESS_H
#define SETTHREATASSESS_H

#include <QString>
#include <QWidget>

#include "RZThreatAssess.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;
class QProgressBar;
class QPushButton;
namespace Ui
{
    class RZThreatAssess;
    class SetThreatAssess;
}
QT_END_NAMESPACE

/** 与 combo 顺序绑定的常数与乘子（表格威胁等级、底部有效威胁度共用） */
namespace ThreatAssessUi
{
    constexpr int kDefaultPositionDefenseComboIndex = 1;
    constexpr int kDefaultRadiationModeComboIndex = 1;
    double radiationModeMultiplier(int comboIndex);
    double clamp01(double value);
}

/**
 * 绑定 horizontalLayout_3 内控件指针；可从 Ui::RZThreatAssess 或 Ui::SetThreatAssess 附着（两套 .ui 控件名一致）。
 */
class ThreatAssessFormBridge
{
public:
    ThreatAssessFormBridge() = default;

    void attachToRZUi(Ui::RZThreatAssess *ui);
    void attachToSetUi(Ui::SetThreatAssess *ui);

    QPushButton *btnCalculate() const
    {
        return m_btnCalculate;
    }

    QComboBox *comboPositionDefense() const
    {
        return m_comboPositionDefense;
    }

    QComboBox *comboRadiationMode() const
    {
        return m_comboRadiationMode;
    }

    QDoubleSpinBox *spinResultFreqCenter() const
    {
        return m_spinResultFreqCenter;
    }

    QDoubleSpinBox *spinResultPwCenter() const
    {
        return m_spinResultPwCenter;
    }

    QDoubleSpinBox *spinResultPrfCenter() const
    {
        return m_spinResultPrfCenter;
    }

    QDoubleSpinBox *spinResultFFreq() const
    {
        return m_spinResultFFreq;
    }

    QDoubleSpinBox *spinResultFPrf() const
    {
        return m_spinResultFPrf;
    }

    QDoubleSpinBox *spinResultFPw() const
    {
        return m_spinResultFPw;
    }

    QDoubleSpinBox *spinResultF1Raw() const
    {
        return m_spinResultF1Raw;
    }

    QDoubleSpinBox *spinResultRangeMod() const
    {
        return m_spinResultRangeMod;
    }

    QDoubleSpinBox *spinResultF1() const
    {
        return m_spinResultF1;
    }

    RadarPerformancePara readInputFromUi() const;
    RadarTypicalPara readRepresentativeCentersFromUi() const;
    GlobalThreatFactors readSubfactorSynthesisFromUi() const;

    void syncSubfactorSpinBoxes(const GlobalThreatFactors &factors);
    void syncRepresentativeCentersToUi(const RadarTypicalPara &representative, const RadarPerformancePara &performance,
                                       const RadarThreatAssessResult &fallbackResult);

    void displayResult(const RadarThreatAssessResult &assessment);

    /** 态势摘要、有效威胁度、进度条；若 lastResult 有效则把 F1 同步为当前 spinResultF1 */
    void refreshSituationSummaryEffectiveProgress(RadarThreatAssessResult *lastResult);

    void clearTargetEdits();
    void clearEquipmentEntityEdit();

    void setEditRadarModel(const QString &text);
    void setEditEquipmentEntity(const QString &text);

    void setFreqMin(double v);
    void setFreqMax(double v);
    void setPwMin(double v);
    void setPwMax(double v);
    void setPrfMin(double v);
    void setPrfMax(double v);
    void setRangeKm(double v);

    void setDefenseComboIndex(int index);
    void setRadiationComboIndex(int index);

    int defenseComboCurrentIndex() const;
    int radiationComboCurrentIndex() const;

    bool isBlockingResultSpinSignals() const
    {
        return m_blockResultSpinSignals;
    }

private:
    QLineEdit *m_editRadarModel = nullptr;
    QLineEdit *m_editEquipmentEntity = nullptr;
    QComboBox *m_comboPositionDefense = nullptr;
    QComboBox *m_comboRadiationMode = nullptr;
    QDoubleSpinBox *m_spinRadPowerKw = nullptr;
    QDoubleSpinBox *m_spinRatedPowerKw = nullptr;

    QDoubleSpinBox *m_spinFreqMin = nullptr;
    QDoubleSpinBox *m_spinFreqMax = nullptr;
    QDoubleSpinBox *m_spinPwMin = nullptr;
    QDoubleSpinBox *m_spinPwMax = nullptr;
    QDoubleSpinBox *m_spinPrfMin = nullptr;
    QDoubleSpinBox *m_spinPrfMax = nullptr;
    QDoubleSpinBox *m_spinRangeKm = nullptr;

    QPushButton *m_btnCalculate = nullptr;

    QDoubleSpinBox *m_spinResultFreqCenter = nullptr;
    QDoubleSpinBox *m_spinResultPwCenter = nullptr;
    QDoubleSpinBox *m_spinResultPrfCenter = nullptr;
    QDoubleSpinBox *m_spinResultFFreq = nullptr;
    QDoubleSpinBox *m_spinResultFPrf = nullptr;
    QDoubleSpinBox *m_spinResultFPw = nullptr;
    QDoubleSpinBox *m_spinResultF1Raw = nullptr;
    QDoubleSpinBox *m_spinResultRangeMod = nullptr;
    QDoubleSpinBox *m_spinResultF1 = nullptr;

    QLabel *m_lblSituationSummary = nullptr;
    QLabel *m_lblEffectiveVal = nullptr;
    QProgressBar *m_progressF1 = nullptr;

    bool m_blockResultSpinSignals = false;
};

class SetThreatAssess : public QWidget
{
    Q_OBJECT

public:
    explicit SetThreatAssess(QWidget *parent = nullptr);
    ~SetThreatAssess() override;

    ThreatAssessFormBridge &formBridge()
    {
        return m_form;
    }

    const ThreatAssessFormBridge &formBridge() const
    {
        return m_form;
    }

private:
    Ui::SetThreatAssess *ui;
    ThreatAssessFormBridge m_form;
};

#endif // SETTHREATASSESS_H
