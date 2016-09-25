#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QtWidgets>

#include "globals.h"

class Database{

private:

    Database(){} // make class abstract

public:

    static const QSqlError openDatabase(QString dbname, QSqlDatabase &db, QSqlQuery &q)
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbname);

        if(!db.open())
            return db.lastError();
        else
            qInfo() << "Database opened successfully: " << dbname;

        q = QSqlQuery(db); // initialize query

        return QSqlError();
    }


    static void closeDatabase(QSqlDatabase &db)
    {
        QString connection = db.connectionName();
        db.close();
        db = QSqlDatabase();
        db.removeDatabase(connection);
    }


    static bool createEmployeeTable(QSqlQuery &q)
    {
        char line [100];
        sprintf(line, "name VARCHAR(%d), ssn VARCHAR(%d) PRIMARY KEY", MAX_EMPLOYEE_NAME_LEN, MAX_EMPLOYEE_SSN_LEN);
        QString query = "CREATE TABLE employees(";
        query += line;
        query += ",employeeType VARCHAR(10)";
        query += ",monthlyCompensation DOUBLE PRECISION";
        query += ",hourlyCompensation DOUBLE PRECISION, doneHours DOUBLE PRECISION";
        query += ",realizedOutcome DOUBLE PRECISION, bonus INTEGER, outcomeClaim BOOLEAN";
        query += ",calculatedSalary DOUBLE PRECISION";
        query += ")";
        return q.exec(query);
    }

    static bool addMonthlyEmployee(QSqlQuery &q, const QString &name, const QString &ssn,
                            double monthlyCompensation)
    {
        if (!q.prepare(QString("INSERT INTO employees(name, ssn, employeeType, monthlyCompensation, calculatedSalary) VALUES(?, ?, ?, ?, ?)")))
            return false;

        q.addBindValue(name);
        q.addBindValue(ssn);
        q.addBindValue(etypeToQstr(employeeTypes::Monthly));
        q.addBindValue(monthlyCompensation);
        q.addBindValue(monthlyCompensation);
        return q.exec();
    }

    static bool addHourlyEmployee(QSqlQuery &q, const QString &name, const QString &ssn,
                           double hourlyCompensation, double doneHours)
    {
        if (!q.prepare(QString("INSERT INTO employees(name, ssn, employeeType, hourlyCompensation, doneHours, calculatedSalary) VALUES(?, ?, ?, ?, ?, ?)")))
            return false;

        q.addBindValue(name);
        q.addBindValue(ssn);
        q.addBindValue(etypeToQstr(employeeTypes::Hourly));
        q.addBindValue(hourlyCompensation);
        q.addBindValue(doneHours);
        q.addBindValue(doneHours*hourlyCompensation);
        return q.exec();
    }

    static bool addSalesmanEmployee(QSqlQuery &q, const QString &name, const QString &ssn,
                             double monthlyCompensation, double realizedOutcome, int bonus, bool outcomeClaim)
    {
        if (!q.prepare(QString("INSERT INTO employees(name, ssn, employeeType, monthlyCompensation, realizedOutcome, bonus, outcomeClaim, calculatedSalary) VALUES(?, ?, ?, ?, ?, ?, ?, ?)")))
            return false;

        q.addBindValue(name);
        q.addBindValue(ssn);
        q.addBindValue(etypeToQstr(employeeTypes::Salesman));
        q.addBindValue(monthlyCompensation);
        q.addBindValue(realizedOutcome);
        q.addBindValue(bonus);
        q.addBindValue(outcomeClaim);
        q.addBindValue( (1.0+(double)outcomeClaim*bonus/100.0) * monthlyCompensation);
        return q.exec();
    }

};


#endif // DATABASE_H
