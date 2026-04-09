//
// Created by admin on "2026.04.08 T 15:03:39".
//

#ifndef SOURCERADIATION_H
#define SOURCERADIATION_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>

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

public:


private:
    // 初始化雷达数据
    void initRadarData();
    // 初始化电台数据
    void initRadioData();
    // 初始化通信对抗数据
    void initComJamData();
    // 初始化雷达对抗数据
    void initRadarJamData();

    // 设置按钮样式
    void setButtonStyle(const QString& activeButton);

private:
    Ui::SourceRadiation *ui;
    
    // 动画组
    QPropertyAnimation *radarAnim;
    QPropertyAnimation *radioAnim;
    QPropertyAnimation *comJamAnim;
    QPropertyAnimation *radarJamAnim;
};

#endif //SOURCERADIATION_H
