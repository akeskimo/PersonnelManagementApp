
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>

#include "employeedelegate.h"
#include "employeeeditor.h"
#include "mainwindow.h"


EmployeeDelegate::EmployeeDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{

}

EmployeeDelegate::~EmployeeDelegate()
{

}

QWidget* EmployeeDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    int row = index.row();

    qDebug() << "Current row: " << row;

    EmployeeEditor* editor = new EmployeeEditor(parent);

    if (editor) {
        editor->setHeader("Edit Employee");
        connect(editor, SIGNAL(editingAccepted()), this, SLOT(handleCommitAndCloseEditor()));
//        connect(editor, &EmployeeEditor::editingAccepted,
//                this, &EmployeeDelegate::handleCommitAndCloseEditor);
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void EmployeeDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    qDebug() << "Setting editor data: " << index.data();

    int row = index.row();

    EmployeeEditor* edr = qobject_cast<EmployeeEditor*>(editor);

    if (edr)
    {
        if (!edr->getDataSet())
        {
            // align the editor on top of the parent
            edr->move(edr->parentWidget()->mapToGlobal(QPoint(0,0)));

            // set editor data
            edr->setLineEditName( index.sibling(row,0).data().toString() );
            edr->setLineEditSsn( index.sibling(row,1).data().toString() );
            edr->setLineEditSsnDisable(true);
            edr->setLineEditPayType( qstrToEtype(index.sibling(row,2).data().toString()) );
            edr->setLineEditPayTypeDisable(true);
            edr->setLineEditMonthly( index.sibling(row,3).data().toDouble() );
            edr->setLineEditHourly( index.sibling(row,4).data().toDouble() );
            edr->setLineEditDoneHours( index.sibling(row,5).data().toDouble() );
            edr->setLineEditRealizedOutcome( index.sibling(row,6).data().toDouble() );
            edr->setLineEditBonus( index.sibling(row,7).data().toDouble() );
            edr->setLineEditOutcomeClaim( index.sibling(row,8).data().toBool() );

            edr->setDataSet(true);
        }
    } else {
        qWarning("Unable to down-cast editor widget!");
    }
}

void EmployeeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    qDebug() << "Model received: " << index.row();

    EmployeeEditor* edr = qobject_cast<EmployeeEditor*>(editor);
    if (edr)
    {
        QSqlRelationalTableModel* mdl = qobject_cast<QSqlRelationalTableModel*>(model);
        if (mdl)
        {
            qDebug("Populating model with new data from editor...");
            edr->updateModelWithEditorData(mdl, index);
        }
        else {
            qCritical("Unable to cast model!");
        }

    } else
    {
        qWarning("Unable to cast editor widget!");
    }
}

void EmployeeDelegate::handleCommitAndCloseEditor()
{
    qDebug() << "Commit and close called!";
    EmployeeEditor *editor = qobject_cast<EmployeeEditor *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
