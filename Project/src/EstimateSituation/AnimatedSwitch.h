#ifndef ANIMATEDSWITCH_H
#define ANIMATEDSWITCH_H

#include <QCheckBox>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QEasingCurve>
#include <QStyle>
#include <QPaintEvent>

class AnimatedSwitch : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(qreal handlePos READ handlePos WRITE setHandlePos)

public:
    explicit AnimatedSwitch(QWidget *parent = nullptr);
    qreal handlePos() const { return m_handlePos; }
    void setHandlePos(qreal v);

protected:
    void nextCheckState() override;
    void paintEvent(QPaintEvent *e) override;

private:
    qreal m_handlePos;
};

#endif // ANIMATEDSWITCH_H
