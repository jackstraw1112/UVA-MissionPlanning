#ifndef PATHDISPLAYDIALOG_H
#define PATHDISPLAYDIALOG_H

#include <QDialog>
#include "StructData.h"

namespace Ui {
class PathDisplayDialog;
}

class PathDisplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathDisplayDialog(QWidget *parent = nullptr);
    ~PathDisplayDialog();

    void setPathData(const PathPlanning& path);

private:
    Ui::PathDisplayDialog *ui;
};

#endif // PATHDISPLAYDIALOG_H
