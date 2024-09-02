#ifndef KINEMATICMODELEDITOR_H
#define KINEMATICMODELEDITOR_H

#include <QWidget>
#include <QAbstractItemDelegate>
#include <QModelIndex>

namespace Ui {
class KinematicLoadingModelEditor;
}

class KinematicLoadingModel;
class KinematicPressurisedLoadingModel;

class KinematicLoadingModelEditor : public QWidget
{
    Q_OBJECT

public:
    explicit KinematicLoadingModelEditor(QWidget *parent = nullptr);
    ~KinematicLoadingModelEditor();

    void setModel(KinematicLoadingModel *model);
    void setModel(KinematicPressurisedLoadingModel *model);
    void setModel(QAbstractTableModel *model);

private:
    Ui::KinematicLoadingModelEditor *ui;
    QAbstractTableModel *m_model;
    //KinematicPressurisedLoadingModel *m_modelPressure;
    //KinematicLoadingModel *m_model;
};

#endif // KINEMATICMODELEDITOR_H
