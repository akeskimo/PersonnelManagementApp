
#include <QApplication>
#include <QtCore>
#include <QLocale>

#include "mainwindow.h"
#include "globals.h"

#include <stdlib.h>
#include <stdio.h>


// message handler for formatting Qt log messages
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    QDateTime dateTime(QDateTime::currentDateTime());
    QString timeStr(dateTime.toString("dd-MM-yyyy HH:mm:ss:zzz"));
    QString contextString(QString("(%1, %2, %3): ").arg(context.file).arg(context.function).arg(context.line));

    QFile outFile("runtime.log");
    if( outFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&outFile);

        switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            stream << timeStr << " - DEBUG - " << contextString << localMsg.constData() << endl;
            break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            stream << timeStr << " - INFO - " << contextString << localMsg.constData() << endl;
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            stream << timeStr << " - WARNING - " << contextString << localMsg.constData() << endl;
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            stream << timeStr << " - CRITICAL - " << contextString << localMsg.constData() << endl;
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            stream << timeStr << " - FATAL - " << contextString << localMsg.constData() << endl;
        }
    }
    else
        fprintf(stderr, "Unable to open log file for writing!");
}


int main(int argc, char *argv[])
{
    qInfo("Application started");

    QLocale::setDefault(QLocale::c());

    QApplication a(argc, argv);

    qInstallMessageHandler(customMessageHandler);

    MainWindow w;
    w.show();

    return a.exec();
}
