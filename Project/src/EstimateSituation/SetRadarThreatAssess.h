//
// SetThreatAssess：承载「辐射源信息 + 评估结果」等界面；Ui::SetThreatAssess 仅在本类 cpp 中使用，不对外暴露。
//

#ifndef SETTHREATASSESS_H
#define SETTHREATASSESS_H

#include <QString>
#include <QWidget>

#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SetRadarThreatAssess;
}
QT_END_NAMESPACE

namespace ThreatAssessUi
{
    constexpr int kDefaultPositionDefenseComboIndex = 1;
    constexpr int kDefaultRadiationModeComboIndex = 1;
    double radiationModeMultiplier(int comboIndex);
    double clamp01(double value);
}

class SetRadarThreatAssess : public QWidget
{
    Q_OBJECT

public:
    explicit SetRadarThreatAssess(QWidget *parent = nullptr);
    ~SetRadarThreatAssess() override;

    RadarPerformancePara readInputFromUi() const;
    RadarTypicalPara readRepresentativeCentersFromUi() const;
    RadarThreatFactors readSubfactorSynthesisFromUi() const;

    void displaySubfactor(const RadarThreatFactors &factors);
    void syncRepresentativeCentersToUi(const RadarTypicalPara &representative, const RadarPerformancePara &performance,
                                       const RadarThreatAssessResult &fallbackResult);

    void displayResult(const RadarThreatAssessResult &assessment);
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

    double currentResultF1SpinValue() const;

    bool isBlockingResultSpinSignals() const { return m_blockResultSpinSignals; }

signals:
    void sigEvaluteResult();

public slots:
    void onCalculate();
    void onConfirm();
    void onCancel();

private:
    void connectPanelWidgetRelays();

    Ui::SetRadarThreatAssess *ui;
    bool m_blockResultSpinSignals = false;
};

#endif // SETTHREATASSESS_H
