#ifndef DIFFICULTYWINDOW_H
#define DIFFICULTYWINDOW_H

#include <QDialog>

namespace Ui {
class difficultywindow;
}

class difficultywindow : public QDialog
{
    Q_OBJECT

public:
    explicit difficultywindow(QWidget *parent = 0);
    ~difficultywindow();
    int level();

private slots:
    void on_difficulty_scale_valueChanged(int value);
    void on_difficulty_start_clicked();

private:
    Ui::difficultywindow *ui;
};

#endif // DIFFICULTYWINDOW_H
