//
// Created by 15955 on 2026/4/8.
//

#ifndef UVA_MISSIONPLANNING_SITUATIONCONTROL_H
#define UVA_MISSIONPLANNING_SITUATIONCONTROL_H

#include <QWidget>
#include "AnimatedSwitch.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SituationControl;
}
QT_END_NAMESPACE

class SituationControl : public QWidget
{
    Q_OBJECT

public:
    explicit SituationControl(QWidget *parent = nullptr);
    ~SituationControl() override;

private:
    Ui::SituationControl *ui;
};

#endif // UVA_MISSIONPLANNING_SITUATIONCONTROL_H
