//
// Created by admin on "2026.04.09 T 19:19:20".
//

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

/**
 * @brief 动画开关控件
 * @details 实现了一个带有平滑动画效果的开关控件，继承自 QCheckBox。
 */
class AnimatedSwitch : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(qreal handlePos READ handlePos WRITE setHandlePos)

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit AnimatedSwitch(QWidget *parent = nullptr);

    /**
     * @brief 获取滑块位置
     * @return 滑块位置值
     */
    qreal handlePos() const { return m_handlePos; }

    /**
     * @brief 设置滑块位置
     * @param v 滑块位置值
     */
    void setHandlePos(qreal v);

protected:
    /**
     * @brief 切换检查状态
     * @details 重写父类方法，添加动画效果
     */
    void nextCheckState() override;

    /**
     * @brief 绘制事件
     * @param e 绘制事件对象
     */
    void paintEvent(QPaintEvent *e) override;

private:
    qreal m_handlePos; ///< 滑块位置
};

#endif // ANIMATEDSWITCH_H
