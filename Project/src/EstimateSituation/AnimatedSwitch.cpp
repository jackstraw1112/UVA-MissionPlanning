//
// Created by admin on "2026.04.09 T 19:19:20".
//

#include "AnimatedSwitch.h"
#include <QPainter>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
AnimatedSwitch::AnimatedSwitch(QWidget *parent)
    : QCheckBox(parent), m_handlePos(0)
{
    // 移除样式表设置
}

/**
 * @brief 设置滑块位置
 * @param v 滑块位置值
 * @details 更新滑块位置并触发重绘
 */
void AnimatedSwitch::setHandlePos(qreal v)
{
    m_handlePos = v;
    style()->unpolish(this);
    style()->polish(this);
    update();
}

/**
 * @brief 切换检查状态
 * @details 重写父类方法，添加平滑动画效果
 */
void AnimatedSwitch::nextCheckState()
{
    setChecked(!isChecked());
    
    QPropertyAnimation *anim = new QPropertyAnimation(this, "handlePos");
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    // 根据当前状态设置动画起始和结束值
    if (isChecked()) {
        anim->setStartValue(0);
        anim->setEndValue(1);
    } else {
        anim->setStartValue(1);
        anim->setEndValue(0);
    }
    
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

/**
 * @brief 绘制事件
 * @param e 绘制事件对象
 * @details 绘制开关的滑块
 */
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
