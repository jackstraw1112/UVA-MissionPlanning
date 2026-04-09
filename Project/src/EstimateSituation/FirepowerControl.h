//
// Created by admin on "2026.04.09 T 00:00:00".
//

#ifndef FIREPOWERCONTROL_H
#define FIREPOWERCONTROL_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class FirepowerControl; }
QT_END_NAMESPACE

class FirepowerControl : public QWidget
{
    Q_OBJECT

public:
    explicit FirepowerControl(QWidget *parent = nullptr);
    ~FirepowerControl() override;

private:
    Ui::FirepowerControl *ui;
};


#endif //FIREPOWERCONTROL_H