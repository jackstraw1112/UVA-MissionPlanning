//
// Created by admin on "2026.04.08 T 14:58:56".
//

#ifndef ESTIMATESITUATION_H
#define ESTIMATESITUATION_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class EstimateSituation; }
QT_END_NAMESPACE

/**
 * @brief 态势估计主窗口
 * @details 主应用窗口，包含各种态势估计相关的子窗口。
 */
class EstimateSituation : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit EstimateSituation(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~EstimateSituation() override;

private:
    Ui::EstimateSituation *ui; ///< UI对象指针
};


#endif //ESTIMATESITUATION_H
