//
// Created by admin on "2026.04.08 T 15:03:39".
//

// You may need to build the project (run Qt uic code generator) to get "ui_SourceRadiation.h" resolved

#include "SourceRadiation.h"
#include "ui_SourceRadiation.h"

SourceRadiation::SourceRadiation(QWidget *parent)
    : QWidget(parent), ui(new Ui::SourceRadiation)
{
    ui->setupUi(this);
}

SourceRadiation::~SourceRadiation()
{
    delete ui;
}
