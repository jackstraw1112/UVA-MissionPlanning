//
// Created by admin on "2026.04.08 T 14:58:56".
//

#ifndef ESTIMATESITUATION_H
#define ESTIMATESITUATION_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class EstimateSituation; }
QT_END_NAMESPACE

class EstimateSituation : public QWidget
{
    Q_OBJECT

public:
    explicit EstimateSituation(QWidget *parent = nullptr);
    ~EstimateSituation() override;

private:
    Ui::EstimateSituation *ui;
};


#endif //ESTIMATESITUATION_H
