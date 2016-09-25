#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set main window settings
    QApplication::setApplicationName("PersonnelManagementApp");
    this->setWindowTitle(QApplication::applicationName());

    // load app settings
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                "QtDevelopment", QApplication::applicationName());
    loadSettings();

    if(m_settings->value("MainWindow/pos") == QVariant() ) {
        qDebug("Initial setup detected!");
        this->move(mapFromGlobal(QPoint(300,300)));
        this->resize(1200,350);
        qDebug("Moved window to initial setup position");
    }

    // initialize widgets and model
    initWidgets();
    initSqlModel();
    hideUiExtraWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_delegate;
    delete m_model;
    delete m_settings;
}

void MainWindow::initWidgets()
{
    qDebug("Creating widgets");

    connect(ui->pushButtonEmEdit, SIGNAL (released()), this, SLOT (handleEditClick()) );
    connect(ui->pushButtonEmNew, SIGNAL (released()), this, SLOT (handleNewClick()) );
    connect(ui->pushButtonHideShowCols, SIGNAL(clicked(bool)), this, SLOT(handleShowHideCols()));
    connect(ui->pushButtonEmDelete, SIGNAL(clicked(bool)), this, SLOT(handleDeleteClick()));

    hideUiExtraWidgets();
}

void MainWindow::initSqlModel() {
    qDebug() << "Init sqlModel, loaded sql drivers: " << QSqlDatabase::drivers();

    // check if sql drivers are loaded
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(this, "Unable to load SQL database", "QSQLITE driver is not available");
        exit(EXIT_FAILURE);
    }

    // load data and initialize database name
    dbname = QApplication::applicationDirPath() + "/" + QApplication::applicationName() + ".sqlite3";
    QSqlError err = loadDatabase(dbname);
    if (err.type() != QSqlError::NoError) {
      QMessageBox::critical(this, "Failed to initialize SQL database", err.text());
      exit(EXIT_FAILURE);
    }

    // create data model
    m_model = new QSqlRelationalTableModel(ui->tableView);
    m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_model->setTable("employees");

    // set db headers
    m_model->setHeaderData(m_model->fieldIndex("name"), Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(m_model->fieldIndex("ssn"), Qt::Horizontal, tr("SSN"));
    m_model->setHeaderData(m_model->fieldIndex("employeeType"), Qt::Horizontal, tr("Pay Type"));
    m_model->setHeaderData(m_model->fieldIndex("monthlyCompensation"), Qt::Horizontal, tr("Monthly Salary"));
    m_model->setHeaderData(m_model->fieldIndex("hourlyCompensation"), Qt::Horizontal, tr("Hourly Salary"));
    m_model->setHeaderData(m_model->fieldIndex("doneHours"), Qt::Horizontal, tr("Done Hours"));
    m_model->setHeaderData(m_model->fieldIndex("realizedOutcome"), Qt::Horizontal, tr("Realized Outcome"));
    m_model->setHeaderData(m_model->fieldIndex("bonus"), Qt::Horizontal, tr("Bonus"));
    m_model->setHeaderData(m_model->fieldIndex("outcomeClaim"), Qt::Horizontal, tr("Outcome Claim"));
    m_model->setHeaderData(m_model->fieldIndex("calculatedSalary"), Qt::Horizontal, tr("Calculated Salary"));

    // populate sqlmodel with data
    if (!m_model->select()) {
      QMessageBox::critical(this, "Unable to populate SQL model", m_model->lastError().text());
      return;
    }

    // attach sql model to view
    ui->tableView->setModel(m_model);

    // attach delegate to view for editing
    m_delegate = new EmployeeDelegate(this);
    ui->tableView->setItemDelegate(m_delegate);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed );
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->labelDataTitle->setText("Database: " + QFileInfo(m_model->database().databaseName()).fileName());
    qDebug() << "Setting database name: " << QFileInfo(m_model->database().databaseName()).fileName();
}

QVector<QLineEdit*> MainWindow::getLineEditsInLayout(QLayout *l)
{
    QVector<QLineEdit*> vec;
    qDebug() << "Printing editor grid layout children...";
    for (int i = 0; i < l->count(); i++)
    {
        QLineEdit* o = qobject_cast<QLineEdit*>(l->itemAt(i)->widget());
        if (o) {
            vec.append(o);
        }
    }
    return vec;
}

void MainWindow::handleHelpClick() 
{
    qDebug("Help clicked!");
}
void MainWindow::handleDeleteClick() 
{
    qDebug("Delete clicked!");

    int row = ui->tableView->currentIndex().row();
    qDebug() << "Employee selection changed: " << row;
    currentEmployeeSsn = m_model->data(m_model->index(row,1)).toString();

    int ans = QMessageBox::question(this, "Delete Employee", "Are you sure you want to delete employee (SSN: " + currentEmployeeSsn + ")?");
    if (ans == QMessageBox::Yes) {
        m_model->removeRow(row);
        m_model->select();
    }

}

void MainWindow::handleActionSaveClick() 
{
    qDebug("Action save clicked!");
}
void MainWindow::handleActionLoadClick() 
{
    qDebug("Action load clicked!");
}
void MainWindow::handleActionExitClick() 
{
    qDebug("Action exit clicked!");
}

void MainWindow::handleNewClick() 
{
    qDebug("New clicked!");

    m_editor = new EmployeeEditor(this);
    m_editor->setHeader("New Employee");
    m_editor->setAddMode(true);
    connect(m_editor, SIGNAL(requestAddFromMainWindow()), this, SLOT(handleAddFromMainWindow()));
    connect(this, SIGNAL(requestAddToModel(QSqlRelationalTableModel*,QSqlDatabase&,QSqlQuery&)),
            m_editor, SLOT(handleAddToModel(QSqlRelationalTableModel*,QSqlDatabase&,QSqlQuery&)));
    m_editor->show();
}

void MainWindow::handleAddFromMainWindow()
{
    emit requestAddToModel(m_model, db, q);
    disconnect(m_editor, SLOT(handleAddToModel(QSqlRelationalTableModel*,QSqlDatabase&,QSqlQuery&)) );
    disconnect(this, SLOT(handleAddFromMainWindow()));
    qDebug() << "Requested add to model from editor";
}

void MainWindow::handleEditClick() 
{
    qDebug("Edit clicked!");
    QMessageBox::information(this, "Edit Employee", "Double-clicking any table row will open the Editor dialog.");
}

QSqlError MainWindow::loadDatabase(QString dbname)
{
    bool result = true;

    // open database
    if (Database::openDatabase(dbname, db, q) != QSqlError())
        return q.lastError();

    // check whether employee table already exist in database
    if (db.tables().contains("employees", Qt::CaseInsensitive)) {
        return QSqlError();
    } else {
        QMessageBox::information(this, "Database " + db.databaseName(),
                                 "No previous database found. Initialized new database with example employees.");
        // create employee table
        if (result)
            result = Database::createEmployeeTable(q);

        // add employees
        if(result)
            result = Database::addMonthlyEmployee(q, QString("Teemu Samuli Kullervo"), QString("120384-1233"),3350.99);
        if(result)
            result = Database::addHourlyEmployee(q, QString("Jaana Janita Sannila"), QString("130394-837X"), 21.50, 179.60);
        if(result)
            result = Database::addSalesmanEmployee(q, QString("Juppi Eemeli Järvinen"), QString("301278-123A"), 1500.0, 300.0, 35, true);
        if(result)
            result = Database::addMonthlyEmployee(q, QString("Tiril Sjåstad Christiansen"), QString("010172-671Y"), 5965);
        if(result)
            result = Database::addSalesmanEmployee(q, QString("李小龍 The Little Phoenix"), QString("271140-1337"), 5510.95, 0.0, 100, false);
        if (result)
            qInfo("Tables created successfully");
        else
            qCritical("Unable to create tables");
    }

    return q.lastError();
}


void MainWindow::handleEmployeeSelectionChanged()
{
    qDebug() << "Employee selection changed";
}

void MainWindow::handleShowHideCols()
{
    if(ui->pushButtonHideShowCols->text().contains("hide", Qt::CaseInsensitive))
    {
        ui->pushButtonHideShowCols->setText("Show all columns");
        hideColumns(false, false, false, true, true, true, true, true , true, false);
    } else
    {
        ui->pushButtonHideShowCols->setText("Hide columns");
        hideColumns(false, false, false, false, false, false, false, false, false, false);
    }

}

QString MainWindow::getCurrentEmployeeSsn()
{
    return currentEmployeeSsn;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug("About to quit!");

    saveSettings();

    if (m_model->isDirty())
    {
        int ans = QMessageBox::question(this, "Model has changed", "Unsubmitted database changes detected. Save changes?");
        switch(ans)
        {
        case QMessageBox::Yes:
            if(!m_model->submitAll()) {
                QMessageBox::critical(this, "Database", "Unable to submit changes: " + m_model->lastError().text());
            }
            break;
        default:
            break;
        }
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::saveSettings()
{
    m_settings->setValue("database", dbname);
    m_settings->beginGroup("MainWindow");
    m_settings->setValue("size", size());
    m_settings->setValue("pos", pos());
    m_settings->endGroup();
    qDebug() << "App settings saved to: " << m_settings->fileName();
}


void MainWindow::loadSettings()
{
    m_settings->beginGroup("MainWindow");
    resize(m_settings->value("size", QSize(600, 400)).toSize());
    move(m_settings->value("pos", QPoint(200, 200)).toPoint());
    m_settings->endGroup();
    qDebug() << "Settings loaded from: " <<  m_settings->fileName();
}

void MainWindow::hideColumns(bool col0, bool col1, bool col2, bool col3, bool col4, bool col5, bool col6, bool col7, bool col8, bool col9)
{
    ui->tableView->setColumnHidden(0,col0);
    ui->tableView->setColumnHidden(1,col1);
    ui->tableView->setColumnHidden(2,col2);
    ui->tableView->setColumnHidden(3,col3);
    ui->tableView->setColumnHidden(4,col4);
    ui->tableView->setColumnHidden(5,col5);
    ui->tableView->setColumnHidden(6,col6);
    ui->tableView->setColumnHidden(7,col7);
    ui->tableView->setColumnHidden(8,col8);
    ui->tableView->setColumnHidden(9,col9);
}

void MainWindow::hideUiExtraWidgets()
{
    ui->pushButtonDbCreate->setHidden(true);
    ui->pushButtonDbReject->setHidden(true);
    ui->pushButtonDbSubmit->setHidden(true);
}
