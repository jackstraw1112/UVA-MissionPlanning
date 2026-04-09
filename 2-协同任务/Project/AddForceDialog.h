#ifndef ADDFORCEDIALOG_H
#define ADDFORCEDIALOG_H

#include <QDialog>
#include "StructData.h"

namespace Ui {
class AddForceDialog;
}

class AddForceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddForceDialog(QWidget *parent = nullptr);
    ~AddForceDialog();

signals:
    void forceAdded(const GroupInfo& groupInfo);

private slots:
    void onOkButtonClicked();
    void onCancelButtonClicked();
    void onCloseButtonClicked();

private:
    Ui::AddForceDialog *ui;
};

#endif // ADDFORCEDIALOG_H
