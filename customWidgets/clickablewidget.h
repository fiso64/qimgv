#ifndef CLICKABLEWIDGET_H
#define CLICKABLEWIDGET_H

#include <QWidget>
#include <QMouseEvent>

class ClickableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClickableWidget(QWidget *parent = 0);

signals:
    void pressed(QPoint);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // CLICKABLEWIDGET_H
