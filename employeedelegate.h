#ifndef EMPLOYEEDELEGATE_H
#define EMPLOYEEDELEGATE_H

#include <QStyledItemDelegate>
#include <QtSql>

class EmployeeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EmployeeDelegate(QObject *parent);
    ~EmployeeDelegate();

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private slots:
    void handleCommitAndCloseEditor();
};


#endif // EMPLOYEEDELEGATE_H
