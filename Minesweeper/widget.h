#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void tile_MouseDown(QMouseEvent *);
    void tile_MouseUp(QMouseEvent *);
    void on_newgame_clicked();
    void timer_tick();

private:
    Ui::Widget *ui;
    void create_new_field();
    void create_new_mines();
    void game_lost(int x, int y);
    void game_won();
    void open_tile(int x, int y);
    void choose_difficulty();
};

#endif // WIDGET_H
