#ifndef TARGETPARAMDIALOG_H
#define TARGETPARAMDIALOG_H

#include <QDialog>
#include "StructData.h"

namespace Ui {
class TargetParamDialog;
}

class TargetParamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TargetParamDialog(QWidget *parent = nullptr);
    ~TargetParamDialog();

    int m_editIndex;
    bool m_isEditMode;
    QString m_planName;
    QString m_coordinationName;

    void setPlanInfo(const QString& planName, const QString& coordinationName);
    void setRadarTarget(const RadarTargetParam& radar);
    void setRadioTarget(const RadioTargetParam& radio);
    void setCommJamming(const CommJammingParam& comm);
    void setRcmJamming(const RcmJammingParam& rcm);

signals:
    void radarTargetAdded(const RadarTargetParam& radar);
    void radarTargetUpdated(const RadarTargetParam& radar, int index);
    void radioTargetAdded(const RadioTargetParam& radio);
    void radioTargetUpdated(const RadioTargetParam& radio, int index);
    void commJammingAdded(const CommJammingParam& comm);
    void commJammingUpdated(const CommJammingParam& comm, int index);
    void rcmJammingAdded(const RcmJammingParam& rcm);
    void rcmJammingUpdated(const RcmJammingParam& rcm, int index);

private slots:
    void onTargetTypeChanged(int index);
    void onOkButtonClicked();
    void onCancelButtonClicked();
    void onCloseButtonClicked();

private:
    Ui::TargetParamDialog *ui;

};

#endif // TARGETPARAMDIALOG_H
