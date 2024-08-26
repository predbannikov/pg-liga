#include "viewdelegate.h"

#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

#include "measurements.h"

/* SpinBox delegate */
SpinBoxDelegate::SpinBoxDelegate(const QString &suffix, double maximum, QObject *parent):
    QStyledItemDelegate(parent),
    m_suffix(suffix),
    m_maximum(maximum)
{
    

    
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    

    Q_UNUSED(option)
    Q_UNUSED(index)

    

    return new QDoubleSpinBox(parent);
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    

    auto *spinBox = qobject_cast<QDoubleSpinBox*>(editor);

    spinBox->setSuffix(" " + m_suffix);
    spinBox->setMaximum(m_maximum);
    spinBox->setDecimals(m_decimals);
    spinBox->setSingleStep(m_singleStep);

    spinBox->setValue(index.data(Qt::EditRole).toDouble());

    
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    

    auto *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
    model->setData(index, spinBox->value());

    
}

QString SpinBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    

    Q_UNUSED(locale)

    if(value.isValid()) 
    {
        
        return QString::number(value.toDouble(), 'f', m_decimals) + " " + m_suffix;
    } 
    else 
    {
        
        return "";
    }

    
}

/* ComboBox delegate */
ComboBoxDelegate::ComboBoxDelegate(const QStringList &items, QObject *parent):
    QStyledItemDelegate(parent),
    m_items(items)
{
    

    
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    

    Q_UNUSED(option)
    Q_UNUSED(index)

    auto *comboBox = new QComboBox(parent);
    comboBox->addItems(m_items);

    

    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    

    auto *comboBox = qobject_cast<QComboBox*>(editor);
    auto idx = index.data().toInt();

    comboBox->setCurrentIndex(idx);

    
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    

    auto *comboBox = qobject_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex());

    
}

QString ComboBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    

    Q_UNUSED(locale)

    auto idx = value.toInt();
    if(idx >= 0 && idx < m_items.count()) 
    {
        
        return m_items.at(idx);
    }
    else 
    {
        
        return "";
    }

    
}

/* TimeInterval delegate */
TimeIntervalDelegate::TimeIntervalDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    
    
}

QWidget *TimeIntervalDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    

    Q_UNUSED(option)
    Q_UNUSED(index)

    auto *lineEdit = new QLineEdit(parent);
    lineEdit->setInputMask("99 ч 99 м 99 с");

    

    return lineEdit;
}

void TimeIntervalDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    

    auto *lineEdit = qobject_cast<QLineEdit*>(editor);
    auto interval = index.data().value<Measurements::TimeInterval>();

    lineEdit->setText(interval.toString());

    
}

void TimeIntervalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    

    auto *lineEdit = qobject_cast<QLineEdit*>(editor);
    model->setData(index, QVariant::fromValue(Measurements::TimeInterval::fromStringShort(lineEdit->text())));

    
}

QString TimeIntervalDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    

    Q_UNUSED(locale)

    if(value.canConvert<Measurements::TimeInterval>()) {
        
        return value.value<Measurements::TimeInterval>().toString();
    } else if(value.canConvert<QString>()) {
        
        return value.toString();
    } else {
        
        return "--";
    }

    
}

/* TimeLongInterval delegate */
TimeLongIntervalDelegate::TimeLongIntervalDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    
    
}

QWidget *TimeLongIntervalDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    

    Q_UNUSED(option)
    Q_UNUSED(index)

    auto *lineEdit = new QLineEdit(parent);
    lineEdit->setInputMask("99 д 99 ч 99 м 99 c");

    

    return lineEdit;
}

void TimeLongIntervalDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    

    auto *lineEdit = qobject_cast<QLineEdit*>(editor);
    auto interval = index.data().value<Measurements::TimeLongInterval>();

    lineEdit->setText(interval.toString());

    
}

void TimeLongIntervalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    

    auto *lineEdit = qobject_cast<QLineEdit*>(editor);
    model->setData(index, QVariant::fromValue(Measurements::TimeLongInterval::fromStringLong(lineEdit->text())));

    
}

QString TimeLongIntervalDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    

    Q_UNUSED(locale)

    if(value.canConvert<Measurements::TimeLongInterval>()) {
        
        return value.value<Measurements::TimeLongInterval>().toString();
    } else if(value.canConvert<QString>()) {
        
        return value.toString();
    } else {
        
        return "--";
    }

    
}

/* Weighted delegate */
WeightedDelegate::WeightedDelegate(const QStringList &weightNames, const QList<int> &decimals, const double &maximum, QObject *parent):
    QStyledItemDelegate(parent),
    m_weightNames(weightNames),
    m_decimals(decimals),
    m_maximum(maximum)
{
    
    
}

QWidget *WeightedDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    

    Q_UNUSED(option)
    Q_UNUSED(index)

    

    return new WeightedDelegateEditor(m_weightNames, m_decimals, parent);
}

void WeightedDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    

    auto *paramEditor = qobject_cast<WeightedDelegateEditor*>(editor);
    const auto data = index.data().value<QPair<int, double>>();

    paramEditor->setMaximum(m_maximum);
    paramEditor->setValue(data.second);
    paramEditor->setWeightType(data.first);

    
}

void WeightedDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    

    auto *paramEditor = qobject_cast<WeightedDelegateEditor*>(editor);
    auto data = index.data().value<QPair<int, double>>();

    data.second = paramEditor->value();
    data.first = paramEditor->weightType();

    model->setData(index, QVariant::fromValue(data));

    
}

QString WeightedDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    

    Q_UNUSED(locale)

    const auto data = value.value<QPair<int, double>>();
    const auto prec = m_decimals.value(data.first, 0);

    

    return QString::number(data.second, 'f', prec) + " " + m_weightNames.value(data.first);
}

/* Weighted delegate editor */
WeightedDelegateEditor::WeightedDelegateEditor(const QStringList &weightNames, const QList<int> &decimals, QWidget *parent):
    QWidget(parent),
    m_layout(new QHBoxLayout(this)),
    m_spinBox(new QDoubleSpinBox(this)),
    m_comboBox(new QComboBox(this)),

    m_decimals(decimals)
{
    

    connect(m_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int idx){
        const auto prec = m_decimals.value(idx, 0);
        m_spinBox->setDecimals(prec);
        m_spinBox->setSingleStep(pow(10.0, -prec));
    });

    m_layout->addWidget(m_spinBox);
    m_layout->addWidget(m_comboBox);

    m_spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_comboBox->addItems(weightNames);

    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    
}

void WeightedDelegateEditor::setValue(double value)
{
    

    m_spinBox->setValue(value);

    
}

void WeightedDelegateEditor::setWeightType(int type)
{
    

    m_comboBox->setCurrentIndex(type);

    
}

void WeightedDelegateEditor::setDecimals(int prec)
{
    

    m_spinBox->setDecimals(prec);
    m_spinBox->setSingleStep(pow(10.0, -prec));

    
}

void WeightedDelegateEditor::setMaximum(double max)
{
    

    m_spinBox->setMaximum(max);

    
}

double WeightedDelegateEditor::value() const
{
    
    

    return m_spinBox->value();
}

int WeightedDelegateEditor::weightType() const
{
    
    

    return m_comboBox->currentIndex();
}

QString SpinBoxDelegateAuto::displayText(const QVariant &value, const QLocale &locale) const
{
    

    if(value.toDouble() == 0) {
        
        return tr("авто");
    } else {
        
        return SpinBoxDelegate::displayText(value, locale);
    }

    
}

SpinBoxDelegateMulti::SpinBoxDelegateMulti(const QStringList &suffixes, const QList<int> &decimals, QObject *parent):
    QStyledItemDelegate(parent),
    m_suffixes(suffixes),
    m_decimals(decimals)
{
    

    
}

QWidget *SpinBoxDelegateMulti::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    

    Q_UNUSED(option)
    Q_UNUSED(index)

    

    return new QDoubleSpinBox(parent);
}

void SpinBoxDelegateMulti::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    

    auto *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
    const auto data = index.data().value<QPair<int, double>>();

    spinBox->setMaximum(999999.0);
    spinBox->setMinimum(1);

    spinBox->setSuffix(" " + m_suffixes.at(data.first));
    spinBox->setDecimals(m_decimals.at(data.first));
    spinBox->setValue(data.second);

    
}

void SpinBoxDelegateMulti::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    

    auto *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
    const auto data = index.data().value<QPair<int, double>>();

    model->setData(index, QVariant::fromValue<QPair<int, double>>({data.first, spinBox->value()}));

    
}

QString SpinBoxDelegateMulti::displayText(const QVariant &value, const QLocale &locale) const
{
    

    Q_UNUSED(locale)

    const auto data = value.value<QPair<int, double>>();
    const auto prec = m_decimals.value(data.first, 0);

    

    return QString::number(data.second, 'f', prec) + " " + m_suffixes.value(data.first);
}
