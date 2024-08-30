#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>
#include <QWidget>
#include <QTabBar>
#include <QMouseEvent>

class CustomTabBar : public QTabBar
{
    Q_OBJECT

public:
    CustomTabBar(QWidget *parent = nullptr) : QTabBar(parent) {}

protected:
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton) {
            int tabIndex = tabAt(event->pos());
            if (tabIndex != 0) { // Первая вкладка неперемещаемая
                QTabBar::mouseMoveEvent(event);
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            int tabIndex = tabAt(event->pos());
            if (tabIndex != 0) { // Первая вкладка неперемещаемая
                QTabBar::mousePressEvent(event);
            }
        }
    }
};
class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    CustomTabWidget(QWidget *parent = nullptr);
//protected:
    QTabBar *createTabBar()
    {
        return new CustomTabBar(this);
    }
};

#endif // CUSTOMTABWIDGET_H
