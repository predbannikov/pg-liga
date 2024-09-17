#ifndef STEPPEDMODELEDITOR_H
#define STEPPEDMODELEDITOR_H

#include <QWidget>
#include <QVector>

namespace Ui {
class SteppedModelEditor;
}

class QStyledItemDelegate;

class AbstractSteppedModel;
class AdvancedSaturationModel;
class ConsolidationModel;
class ConsolidationAltModel;
class SteppedLoadingModel;
class SteppedPressuriseModel;
class AdvancedKinematicLoadingModel;
class RelaxationLoadingModel;
class DeformModel;

class SteppedModelEditor : public QWidget
{
    Q_OBJECT

public:
    SteppedModelEditor(QWidget *parent = nullptr);
    ~SteppedModelEditor();

    void setModel(AdvancedSaturationModel *model);
    void setModel(ConsolidationModel *model);
    void setModel(ConsolidationAltModel *model);
    void setModel(SteppedLoadingModel *model);
    void setModel(SteppedPressuriseModel *model);
    void setModel(AdvancedKinematicLoadingModel *model);
    void setModel(RelaxationLoadingModel *model);
    void setModel(DeformModel *model);

    void addStep();
    void clearStep(int idx);

    QJsonObject serializModel();

signals:
    void dataChanged();

public slots:
    void setStepHighlighted(int idx);

private:
    void setupModel(AbstractSteppedModel *model);
    void setupDelegates(const QVector<QStyledItemDelegate*> &delegates);

    Ui::SteppedModelEditor *ui;
//    AbstractSteppedModel *m_model;
};

#endif // STEPPEDMODELEDITOR_H
