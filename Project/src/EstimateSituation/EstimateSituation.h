//
// Created by admin on "2026.04.08 T 14:58:56".
//

#ifndef ESTIMATESITUATION_H
#define ESTIMATESITUATION_H

#include <QWidget>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class EstimateSituation;
}
QT_END_NAMESPACE

class RZSourceRadiation;
class SpectrumAnalysis;

class FirepowerControl;
class RZThreatAssess;

class EstimateSituation : public QWidget
{
    Q_OBJECT

public:
    explicit EstimateSituation(QWidget *parent = nullptr);
    ~EstimateSituation() override;

public:
    void addData(const RadarPerformancePara &data)
    {
        addDataImpl(data);
    }

    void updateData(const RadarPerformancePara &data)
    {
        updateDataImpl(data);
    }

    void deleteData(const QString &name)
    {
        deleteDataImpl(name);
    }

    void addData(const FirepowerItem &data)
    {
        addDataImpl(data);
    }

    void updateData(const FirepowerItem &data)
    {
        updateDataImpl(data);
    }

    void addData(const SituationControlData &data)
    {
        addDataImpl(data);
    }

    void updateData(const SituationControlData &data)
    {
        updateDataImpl(data);
    }

private:
    void initParams();
    void initObject();
    void initConnect();

private:
    Ui::EstimateSituation *ui;

    RZSourceRadiation *m_rzSourceRadiation;
    SpectrumAnalysis *m_spectrumAnalysis;

    FirepowerControl *m_firepowerControl;
    RZThreatAssess *m_rzThreatAssess;

private:
    void addDataImpl(const RadarPerformancePara &data);
    void addDataImpl(const FirepowerItem &data);
    void addDataImpl(const SituationControlData &data);

    void updateDataImpl(const RadarPerformancePara &data);
    void updateDataImpl(const FirepowerItem &data);
    void updateDataImpl(const SituationControlData &data);

    void deleteDataImpl(const QString &name);
};

#endif // ESTIMATESITUATION_H
