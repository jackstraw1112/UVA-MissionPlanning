//
// Created by admin on "2026.04.08 T 14:58:56".
//

#include "EstimateSituation.h"
#include "ui_EstimateSituation.h"

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
EstimateSituation::EstimateSituation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EstimateSituation)
{
    ui->setupUi(this);
}

/**
 * @brief 析构函数
 */
EstimateSituation::~EstimateSituation()
{
    delete ui;
}
