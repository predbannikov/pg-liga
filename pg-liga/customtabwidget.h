#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>
#include <QWidget>
#include <QTabBar>
#include <QDebug>
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
            if (tabIndex == 0) {
                if (abs(x_hold - event->x()) < 25) {
                    QTabBar::mouseMoveEvent(event);
                } else
                    movingFirstTab = true;

            } else if (tabIndex != 0) { // Первая вкладка неперемещаемая
                QTabBar::mouseMoveEvent(event);
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            int tabIndex = tabAt(event->pos());
            if (tabIndex == 0) {
                x_hold = event->x();
                movingFirstTab = false;
                QTabBar::mousePressEvent(event);
            } else if (tabIndex != 0) { // Первая вкладка неперемещаемая, но можно переключиться на нее
                QTabBar::mousePressEvent(event);
            }
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            QTabBar::mouseReleaseEvent(event);
            if (movingFirstTab) {
                // Восстанавливаем позицию первой вкладки
                for (int i = 0; i < count(); i++) {
                    if (tabText(i) == "Список") {
                        moveTab(i, 0);
                        break;
                    }
                }
            }
            movingFirstTab = false;
        }
    }

private:
    int x_hold = 0;
    bool movingFirstTab = false;
};

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    CustomTabWidget(QWidget *parent = nullptr);

};

#endif // CUSTOMTABWIDGET_H
