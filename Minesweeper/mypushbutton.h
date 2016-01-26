#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>

class MyPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MyPushButton(QWidget *parent = 0);

signals:
    void pressed(QMouseEvent *);
    void released(QMouseEvent *);

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        emit pressed(e);
    }
    void mouseReleaseEvent(QMouseEvent *e)
    {
        emit released(e);
    }

public slots:

};

#endif // MYPUSHBUTTON_H
