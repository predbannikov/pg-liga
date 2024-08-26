#ifndef VIEWDELEGATE_H
#define VIEWDELEGATE_H

#include <QDebug>
#include <QStyledItemDelegate>

class QComboBox;
class QHBoxLayout;
class QDoubleSpinBox;

/* SpinBox delegate */
class SpinBoxDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpinBoxDelegate(const QString &suffix, double maximum, QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;

    void setDecimals(int decimals) 
    {
        m_decimals = decimals; 
    }
    void setSingleStep(double singleStep)
    {
        m_singleStep = singleStep;
    }
    void setMinimum(double minimum) {

    }

private:
    QString m_suffix;
    double m_maximum;

    int m_decimals = 3;
    double m_singleStep = 0.005;
};

/* SpinBox delegate -- displays "auto" if value is zero */
class SpinBoxDelegateAuto : public SpinBoxDelegate
{
    Q_OBJECT

public:
    SpinBoxDelegateAuto(const QString &suffix, double maximum, QObject *parent = nullptr):
        SpinBoxDelegate(suffix, maximum, parent)
    {
    }

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

/* SpinBox delegate -- multiple suffixes */
class SpinBoxDelegateMulti : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpinBoxDelegateMulti(const QStringList &suffixes, const QList<int> &decimals, QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    QStringList m_suffixes;
    QList<int> m_decimals;
};

/* ComboBox delegate */
class ComboBoxDelegate : public QStyledItemDelegate
{
public:
    ComboBoxDelegate(const QStringList &items, QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    QStringList m_items;
};

/* Weighted delegate */
class WeightedDelegate : public QStyledItemDelegate
{
public:
    WeightedDelegate(const QStringList &weightNames, const QList<int> &decimals, const double &maximum, QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    QStringList m_weightNames;
    QList<int> m_decimals;
    double m_maximum;
};

/* Weighted delegate editor */
class WeightedDelegateEditor : public QWidget
{
    Q_OBJECT

public:
    WeightedDelegateEditor(const QStringList &weightNames, const QList<int> &decimals, QWidget *parent = nullptr);

    void setValue(double value);
    void setWeightType(int type);
    void setDecimals(int prec);
    void setMaximum(double max);

    double value() const;
    int weightType() const;

private:
    QHBoxLayout *m_layout;
    QDoubleSpinBox *m_spinBox;
    QComboBox *m_comboBox;

    QList<int> m_decimals;
};

/* TimeInterval delegate */
class TimeIntervalDelegate : public QStyledItemDelegate
{
public:
    TimeIntervalDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

/* TimeLongInterval delegate*/
class TimeLongIntervalDelegate : public QStyledItemDelegate
{
public:
    TimeLongIntervalDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

/* Запрет на редактирование  delegate*/
class NoEditDelegate: public QStyledItemDelegate {
public:
    NoEditDelegate(QObject* parent=0): QStyledItemDelegate(parent) {}
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return 0;
    }
};

#endif // VIEWDELEGATE_H
