#include "AnimatedSwitch.h"
#include <QPainter>

AnimatedSwitch::AnimatedSwitch(QWidget *parent)
    : QCheckBox(parent), m_handlePos(0)
{
    // 移除样式表设置
}

void AnimatedSwitch::setHandlePos(qreal v)
{
    m_handlePos = v;
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void AnimatedSwitch::nextCheckState()
{
    setChecked(!isChecked());
    
    QPropertyAnimation *anim = new QPropertyAnimation(this, "handlePos");
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    if (isChecked()) {
        anim->setStartValue(0);
        anim->setEndValue(1);
    } else {
        anim->setStartValue(1);
        anim->setEndValue(0);
    }
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimatedSwitch::paintEvent(QPaintEvent *e)
{
    QCheckBox::paintEvent(e);
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);

    QRect r = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, this);
    int x = r.x() + 3 + m_handlePos * (r.width() - 14 - 6);
    int y = r.y() + 3;

    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(isChecked() ? QColor("#5BC0FF") : QColor("#8E9CB9"));
    p.drawEllipse(x, y, 14, 14);
}
