//
// Created by admin on "2026.04.08 T 15:03:39".
//

#ifndef SOURCERADIATION_H
#define SOURCERADIATION_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SourceRadiation;
}
QT_END_NAMESPACE

// 辐射源列表
class SourceRadiation : public QWidget
{
    Q_OBJECT

public:
    explicit SourceRadiation(QWidget *parent = nullptr);
    ~SourceRadiation() override;

private:
    Ui::SourceRadiation *ui;
};

#endif //SOURCERADIATION_H
