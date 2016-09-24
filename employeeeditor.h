#ifndef EMPLOYEEEDITOR_H
#define EMPLOYEEEDITOR_H

#include <QDialog>
#include <QDoubleValidator>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSqlRelationalTableModel>
#include <QModelIndex>

#include "globals.h"

namespace Ui
{
class Dialog;
}

class EmployeeEditor : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeEditor(QWidget *parent = 0);
    ~EmployeeEditor();

    bool validateSsn();
    void setDataSet(bool mode);
    bool getDataSet();
    void setAddMode(bool mode);
    void setLineEditSsnDisable(bool mode);
    void setLineEditPayTypeDisable(bool mode);
    void updateModelWithEditorData(QSqlRelationalTableModel *model, const QModelIndex &index);

    void setLineEditName(QString name);
    QString getLineEditName();

    void setLineEditSsn(QString ssn);
    QString getLineEditSsn();

    void setLineEditPayType(employeeTypes typ);
    employeeTypes getLineEditPayType();

    void setLineEditMonthly(double monthly);
    double getLineEditMonthly();

    void setLineEditHourly(double hourly);
    double getLineEditHourly();

    void setLineEditDoneHours(double hours);
    double getLineEditDoneHours();

    void setLineEditBonus(double bonus);
    double getLineEditBonus();

    void setLineEditRealizedOutcome(double realized);
    double getLineEditRealizedOutcome();

    void setLineEditOutcomeClaim(bool claim);
    bool getLineEditOutcomeClaim();

    void setHeader(QString hdr);


public slots:
    void handleAddToModel(QSqlRelationalTableModel *model, QSqlDatabase &db, QSqlQuery &q);

private:
    Ui::Dialog *ui;
    QDoubleValidator *mDouble, *hDouble, *hdDouble, *bDouble, *rDouble;
    bool addmode = false;
    bool dataSet = false;

    void setEditorWidgetVisibility(bool mSal, bool hDone, bool hSal,
                                            bool oClaim, bool bonus, bool real);
    void accept();
    void reject();
    bool checkData(bool showBox = false);

private slots:
    bool handleValidateSsnOnTextChange();
    void handleComboBoxChange();

signals:
    void editingAccepted();
    void requestAddFromMainWindow();

};

#endif // EMPLOYEEEDITOR_H
