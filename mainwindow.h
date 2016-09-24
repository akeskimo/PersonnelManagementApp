#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QIODevice>
#include <QTableView>
#include <QFile>
#include <QtSql>
#include <QMap>

#include "employeeeditor.h"
#include "employeedelegate.h"
#include "database.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString getCurrentEmployeeSsn();
    QVector<QLineEdit*> getLineEditsInLayout(QLayout *l);

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *m_model;
    EmployeeDelegate *m_delegate;
    EmployeeEditor *m_editor;
    QString currentEmployeeSsn = "";
    QSettings *m_settings;
    QString dbname;
    QSqlDatabase db;
    QSqlQuery q;

    void initWidgets();
    void initSqlModel();
    QSqlError loadDatabase(QString dbname);
    void closeEvent(QCloseEvent *event);
    void saveSettings();
    void loadSettings();
    void hideColumns(bool col0, bool col1, bool col2, bool col3, bool col4, bool col5, bool col6, bool col7, bool col8, bool col9);
    void hideUiExtraWidgets();

signals:
    void requestCommitAndCloseEditor();
    void requestAddToModel(QSqlRelationalTableModel *model, QSqlDatabase &db, QSqlQuery &q);

private slots:
    void handleEditClick();
    void handleNewClick();
    void handleHelpClick();
    void handleDeleteClick();
    void handleActionSaveClick();
    void handleActionLoadClick();
    void handleActionExitClick();
    void handleEmployeeSelectionChanged();
    void handleShowHideCols();
    void handleAddFromMainWindow();
};


#endif // MAINWINDOW_H


