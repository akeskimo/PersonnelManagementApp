#ifndef GLOBALS_H
#define GLOBALS_H

// variables
#define MAX_EMPLOYEE_NAME_LEN 30
#define MAX_EMPLOYEE_SSN_LEN 11
//#define EMPLOYEE_SSN_REGEXP "^[0-3]{1}[0-9]{1}[0-1]{1}[0-9]{3}-[0-9]{3}[a-z,A-Z,0-9]{1}$" // XXXXXX-XXXX
#define EMPLOYEE_SSN_REGEXP "^([1-2][0-9]|0[1-9]|3[01])(0[1-9]|1[012])[0-9]{2}-[0-9]{3}[a-z,A-Z,0-9]{1}$"
//#define EMPLOYEE_SSN_REGEXP "^[0-9]{6}-[0-9]{3}[a-z,A-Z,0-9]{1}$" // XXXXXX-XXXX
#define EMPLOYEE_NAME_REGEXP "^[^0-9]{30}" // non-numeric
#define SQLITE_DATABASE_NAME "database"

// structures
enum employeeTypes { None, Monthly, Hourly, Salesman };

// employeeTypes to QString conversion
inline QString etypeToQstr(employeeTypes type)
{
    switch(type) {
    case employeeTypes::None:
        return QString("");
    case employeeTypes::Monthly:
        return QString("Monthly");
    case employeeTypes::Hourly:
        return QString("Hourly");
    case employeeTypes::Salesman:
        return QString("Salesman");
    default:
        return QString("");
    }
}

// QString to employeeType conversion
inline employeeTypes qstrToEtype(QString str)
{
    if(str == QString("Monthly"))
        return employeeTypes::Monthly;
    else if(str == QString("Hourly"))
        return employeeTypes::Hourly;
    else if(str == QString("Salesman"))
        return employeeTypes::Salesman;
    return employeeTypes::None;
}


#endif // GLOBALS_H
