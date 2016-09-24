#include <QRegExp>
#include <QDebug>
#include <QPalette>

#include "employeeeditor.h"
#include "ui_dialog.h"
#include "mainwindow.h"

EmployeeEditor::EmployeeEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // hide widgets that are not needed
    setEditorWidgetVisibility(false, false, false, false, false, false);

    // fill combobox text
    for ( int i = employeeTypes::None; i-1 != employeeTypes::Salesman; i++ )
    {
       employeeTypes typ = static_cast<employeeTypes>(i);
       ui->comboBox->addItem(etypeToQstr(typ));
    }

    // set input validators and hints
    mDouble = new QDoubleValidator(0,9999.99,2,this);
    mDouble->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditMonthlySalary->setValidator(mDouble);

    hDouble = new QDoubleValidator(0,999.99,2,this);
    hDouble->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditHourlySalary->setValidator(hDouble);

    hdDouble = new QDoubleValidator(0,999.99,2,this);
    hdDouble->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditDoneHours->setValidator(hdDouble);

    bDouble = new QDoubleValidator(0,100,2,this);
    bDouble->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditBonus->setValidator(bDouble);

    rDouble = new QDoubleValidator(0,9999.99,2,this);
    rDouble->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEditRealizedOutcome->setValidator(rDouble);

    QValidator *name = new QRegExpValidator(QRegExp(EMPLOYEE_NAME_REGEXP), this);
    ui->lineEditName->setValidator(name);

    QValidator *ssn = new QRegExpValidator(QRegExp(EMPLOYEE_SSN_REGEXP), this);
    ui->lineEditSsn->setValidator(ssn);

    connect(ui->lineEditSsn, SIGNAL(textChanged(QString)), this, SLOT(handleValidateSsnOnTextChange()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleComboBoxChange()));
}

EmployeeEditor::~EmployeeEditor()
{
    delete ui;
    delete mDouble;
    delete hDouble;
    delete hdDouble;
    delete bDouble;
    delete rDouble;
}


bool EmployeeEditor::validateSsn()
{
    QRegExp rx = QRegExp("^[0-9]{6}-[0-9]{3}[a-z,A-Z,0-9]{1}$");
    if (rx.exactMatch(ui->lineEditSsn->text())) {
        return true;
    }
    return false;
}

void EmployeeEditor::setDataSet(bool mode)
{
    this->dataSet = mode;
}

bool EmployeeEditor::getDataSet()
{
    return this->dataSet;
}

bool EmployeeEditor::handleValidateSsnOnTextChange()
{
    QRegExp rx = QRegExp("^[0-9]{6}-[0-9]{3}[a-z,A-Z,0-9]{1}$");
    QPalette palette;

    if (rx.exactMatch(ui->lineEditSsn->text())) {
        qDebug("Data valid!");
        palette.setColor(QPalette::Text, Qt::black);
        ui->lineEditSsn->setPalette(palette);
    } else {
        palette.setColor(QPalette::Text, Qt::red);
        ui->lineEditSsn->setPalette(palette);
    }
    return false;
}

bool EmployeeEditor::checkData(bool showBox)
{
    bool result = true;

    if (result) {
        result = ui->lineEditName->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "Name cannot be empty!");
    }

    if (result) {
        result = ui->lineEditSsn->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "SSN cannot be empty!");
    }

    if (result) {
        result = validateSsn();
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", QString("Invalid SSN: " + ui->lineEditSsn->text()));
    }

    if (result) {
        result = ui->comboBox->currentIndex() != 0;
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "You must select Pay Type!");
    }

    if (result && ui->lineEditMonthlySalary->isVisible()) {
        result = ui->lineEditMonthlySalary->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "Monthly salary cannot be empty!");
    }

    if (result && ui->lineEditHourlySalary->isVisible()) {
        result = ui->lineEditHourlySalary->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "Hourly salary cannot be empty!");
    }

    if (result && ui->lineEditDoneHours->isVisible() ) {
        result = ui->lineEditDoneHours->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "Done hours cannot be empty!");
    }

    if (result && ui->lineEditBonus->isVisible() ) {
        result = ui->lineEditBonus->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "Bonus cannot be empty!");
    }

    if (result && ui->lineEditRealizedOutcome->isVisible() ) {
        result = ui->lineEditRealizedOutcome->text() != QString("");
        if (!result && showBox)
            QMessageBox::information(this, "Unable to save data", "Realized outcome cannot be empty!");
    }

    return result;
}

void EmployeeEditor::setEditorWidgetVisibility(bool mSal, bool hDone, bool hSal,
                                                        bool oClaim, bool bonus, bool real)
{
    ui->labelMonthlySalary->setVisible(mSal);
    ui->lineEditMonthlySalary->setVisible(mSal);
    ui->lineEditMonthlySalary->setText("");
    ui->labelDoneHours->setVisible(hDone);
    ui->lineEditDoneHours->setVisible(hDone);
    ui->lineEditDoneHours->setText("");
    ui->labelHourlySalary->setVisible(hSal);
    ui->lineEditHourlySalary->setVisible(hSal);
    ui->lineEditHourlySalary->setText("");
    ui->labelOutcomeClaim->setVisible(oClaim);
    ui->checkBox->setVisible(oClaim);
    ui->checkBox->setChecked(false);
    ui->labelBonus->setVisible(bonus);
    ui->lineEditBonus->setVisible(bonus);
    ui->lineEditBonus->setText("");
    ui->labelRealized->setVisible(real);
    ui->lineEditRealizedOutcome->setVisible(real);
    ui->lineEditRealizedOutcome->setText("");
}

void EmployeeEditor::updateModelWithEditorData(QSqlRelationalTableModel *model, const QModelIndex &index)
{
    if (!checkData()) {
        qCritical("Unexpected call: Trying to update model with invalid data -> returning");
        return;
    }

    if (addmode)
    {
        qInfo("Editor in add mode!");
        return;
    }

    qDebug("Updating model with editor data...");

    int row = index.row();

    model->setData(index.sibling(row,model->fieldIndex("name")), QVariant(getLineEditName()));
    model->setData(index.sibling(row,model->fieldIndex("ssn")), QVariant(getLineEditSsn()));
    model->setData(index.sibling(row,model->fieldIndex("employeeType")), QVariant(etypeToQstr(getLineEditPayType())));

    employeeTypes typ = getLineEditPayType();

    bool res = false;

    switch(typ)
    {
    case employeeTypes::Monthly:
        model->setData(index.sibling(row,model->fieldIndex("monthlyCompensation")), QVariant(getLineEditMonthly()) );
        model->setData(index.sibling(row,model->fieldIndex("hourlyCompensation")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("doneHours")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("realizedOutcome")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("bonus")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("outcomeClaim")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("calculatedSalary")), QVariant(getLineEditMonthly()));
        break;
    case employeeTypes::Hourly:
    {
        model->setData(index.sibling(row,model->fieldIndex("monthlyCompensation")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("hourlyCompensation")), QVariant(getLineEditHourly()));
        res = model->setData(index.sibling(row,model->fieldIndex("doneHours")), QVariant(getLineEditDoneHours()));
        qDebug() << "Linedit: " << QVariant(getLineEditDoneHours());
        if(res)
            qDebug() << "Updated: " << QVariant(model->data(index.sibling(row,model->fieldIndex("doneHours") )));
        qDebug() << index.sibling(row,model->fieldIndex("doneHours"));
        model->setData(index.sibling(row,model->fieldIndex("realizedOutcome")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("bonus")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("outcomeClaim")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("calculatedSalary")),
                       QVariant(getLineEditHourly() * getLineEditDoneHours()) );
        break;
    }
    case employeeTypes::Salesman:
        model->setData(index.sibling(row,model->fieldIndex("monthlyCompensation")), QVariant(getLineEditMonthly()));
        model->setData(index.sibling(row,model->fieldIndex("hourlyCompensation")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("doneHours")), QVariant());
        model->setData(index.sibling(row,model->fieldIndex("realizedOutcome")), QVariant(getLineEditRealizedOutcome()));
        model->setData(index.sibling(row,model->fieldIndex("bonus")), QVariant(getLineEditBonus()));
        model->setData(index.sibling(row,model->fieldIndex("outcomeClaim")), QVariant(getLineEditOutcomeClaim()));
        model->setData(index.sibling(row,model->fieldIndex("calculatedSalary")),
                       QVariant( (1.0 + (double)getLineEditOutcomeClaim()*getLineEditBonus()/100.0)
                                 * getLineEditMonthly() ) );
        break;
    default:
        break;
    }

    model->select();
    qDebug() << "Model updated!";
}


void EmployeeEditor::handleAddToModel(QSqlRelationalTableModel *model, QSqlDatabase &db, QSqlQuery &q)
{
    qDebug() << "handleAddToModel called!";

    bool result = true;

    if (!db.tables().contains("employees", Qt::CaseInsensitive) )
    {
        if (Database::createEmployeeTable(q))
            qDebug() << "Employee table created";
        else
            QMessageBox::critical(this, "Unable to create table", "SQL error: " + model->lastError().text());
    }

    employeeTypes typ = getLineEditPayType();
    switch(typ)
    {
    case employeeTypes::Monthly:
        result = Database::addMonthlyEmployee(q, getLineEditName(), getLineEditSsn(), getLineEditMonthly());
        break;
    case employeeTypes::Hourly:
        result = Database::addHourlyEmployee(q, getLineEditName(), getLineEditSsn(), getLineEditHourly(), getLineEditDoneHours());
        break;
    case employeeTypes::Salesman:
        result = Database::addSalesmanEmployee(q, getLineEditName(), getLineEditSsn(), getLineEditMonthly(),
                                     getLineEditRealizedOutcome(), getLineEditBonus(), getLineEditOutcomeClaim());
        break;
    default:
        break;
    }

    if(result) {
        QMessageBox::information(this, "New employee", "Employee (SSN: " + getLineEditSsn() + ") added to database");
        model->select();
    }
    else {
        if (q.lastError().text().contains("unique", Qt::CaseInsensitive) ) {
            QMessageBox::critical(this, "Unable to add", "Unable to add employee to database! SSN already exists: " + getLineEditSsn());
        } else {
            qCritical() << "Unhandled SQL error: " << q.lastError();
        }
    }

}

void EmployeeEditor::handleComboBoxChange()
{
    int idx = ui->comboBox->currentIndex();

    employeeTypes typ = static_cast<employeeTypes>(idx);

    switch(typ)
    {
    case employeeTypes::None:
        setEditorWidgetVisibility(false, false, false, false, false, false);
        break;
    case employeeTypes::Monthly:
        setEditorWidgetVisibility(true, false, false, false, false, false);
        break;
    case employeeTypes::Hourly:
        setEditorWidgetVisibility(false, true, true, false, false, false);
        break;
    case employeeTypes::Salesman:
        setEditorWidgetVisibility(true, false, false, true, true, true);
        break;
    default:
        break;
    }

    repaint();

}

void EmployeeEditor::setLineEditName(QString name)
{
    ui->lineEditName->setText(name);
}

QString EmployeeEditor::getLineEditName()
{
    return ui->lineEditName->text();
}


void EmployeeEditor::setLineEditSsn(QString ssn)
{
    ui->lineEditSsn->setText(ssn);
}

QString EmployeeEditor::getLineEditSsn()
{
    return ui->lineEditSsn->text();
}

void EmployeeEditor::setLineEditSsnDisable(bool mode)
{
    ui->lineEditSsn->setDisabled(mode);
}


void EmployeeEditor::setLineEditPayType(employeeTypes typ)
{
    int idx = static_cast<int>(typ);
    ui->comboBox->setCurrentIndex(idx);
}

employeeTypes EmployeeEditor::getLineEditPayType()
{
    employeeTypes typ = static_cast<employeeTypes> (ui->comboBox->currentIndex());
    return typ;
}

void EmployeeEditor::setLineEditPayTypeDisable(bool mode)
{
    ui->comboBox->setDisabled(mode);
}

void EmployeeEditor::setLineEditMonthly(double monthly)
{
    ui->lineEditMonthlySalary->setText(QString::number(monthly));
}

double EmployeeEditor::getLineEditMonthly()
{
    return ui->lineEditMonthlySalary->text().toDouble();
}

void EmployeeEditor::setLineEditHourly(double hourly)
{
    ui->lineEditHourlySalary->setText(QString::number(hourly));
}

double EmployeeEditor::getLineEditHourly()
{
    return ui->lineEditHourlySalary->text().toDouble();
}

void EmployeeEditor::setLineEditDoneHours(double hours)
{
    ui->lineEditDoneHours->setText(QString::number(hours));
}

double EmployeeEditor::getLineEditDoneHours()
{
    return ui->lineEditDoneHours->text().toDouble();
}

void EmployeeEditor::setLineEditBonus(double bonus)
{
    ui->lineEditBonus->setText(QString::number(bonus));
}

double EmployeeEditor::getLineEditBonus()
{
    return ui->lineEditBonus->text().toDouble();
}

void EmployeeEditor::setLineEditRealizedOutcome(double realized)
{
    ui->lineEditRealizedOutcome->setText(QString::number(realized));
}

double EmployeeEditor::getLineEditRealizedOutcome()
{
    return ui->lineEditRealizedOutcome->text().toDouble();
}

void EmployeeEditor::setLineEditOutcomeClaim(bool claim)
{
    ui->checkBox->setChecked(claim);
}

bool EmployeeEditor::getLineEditOutcomeClaim()
{
    return ui->checkBox->isChecked();
}

void EmployeeEditor::setHeader(QString hdr)
{
    ui->labelHeader->setText(hdr);
}

void EmployeeEditor::accept()
{
    if (checkData(true)) {
        if (addmode) {
            emit requestAddFromMainWindow();
            qDebug("sent: requestAddFromMainWindow");
        }
        else {
            emit editingAccepted();
            qDebug("sent: editingAccepted");
        }
        QDialog::accept();
    } else {
        qWarning() << "Accept blocked! checkData(true) = " << checkData(true);
    }
}

void EmployeeEditor::reject()
{
    qDebug("reject = true");
    QDialog::reject();
}

void EmployeeEditor::setAddMode(bool mode)
{
    this->addmode = mode;
}
