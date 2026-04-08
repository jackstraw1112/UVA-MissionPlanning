//
// Created by admin on "2026.04.08 T 14:58:56".
//

// You may need to build the project (run Qt uic code generator) to get "ui_EstimateSituation.h" resolved

#include "EstimateSituation.h"
#include "ui_EstimateSituation.h"

EstimateSituation::EstimateSituation(QWidget *parent)
    : QWidget(parent), ui(new Ui::EstimateSituation)
{
    ui->setupUi(this);
}

EstimateSituation::~EstimateSituation()
{
    delete ui;
}
