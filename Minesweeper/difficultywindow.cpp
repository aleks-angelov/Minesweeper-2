#include "difficultywindow.h"
#include "ui_difficultywindow.h"

difficultywindow::difficultywindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::difficultywindow)
{
    ui->setupUi(this);
}

difficultywindow::~difficultywindow()
{
    delete ui;
}

void difficultywindow::on_difficulty_scale_valueChanged(int value)
{
    ui->difficulty_value->setText((QString::number(value)));
}

int difficultywindow::level()
{
    return ui->difficulty_scale->value();
}

void difficultywindow::on_difficulty_start_clicked()
{
    difficultywindow::accept();
}
