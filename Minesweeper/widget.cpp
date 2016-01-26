#include "widget.h"
#include "ui_widget.h"
#include "mypushbutton.h"
#include "difficultywindow.h"
#include "QGridLayout"
#include "QMessageBox"
#include "QMouseEvent"
#include "QProcess"
#include "QTime"
#include "QTimer"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    create_new_field();
    choose_difficulty();
}

Widget::~Widget()
{
    delete ui;
}

struct tilestruct
{
    short valuestatus, coverstatus;
    /*
    Legend:
    valuestatus:
    -1 - mine tile
    0 - empty tile
    1-8 - non-mine tile with 1-8 adjacent mines
    9 - first clicked tile

    coverstatus:
    -1 - opened status
    0 - default (empty) status
    1 - flagged status
    2 - unsure (?) status
    */
    QLabel *value;
    MyPushButton *cover;
};

tilestruct tiles[18][18];
int mines=40, closedsafetiles=256, time=0;
QIcon *gamelost, *gamewon, *leftclick, *new_game, *tile, *tile_flagged;
QIcon *tile_mine, *tile_mine_flag, *tile_mine_nomine, *tile_unsure;
QPixmap *redmine_broken;
QTimer *timer = new QTimer();

// Generates the 16x16 game field
void Widget::create_new_field()
{
    gamelost=new QIcon(":/images/gamelost.png");
    gamewon=new QIcon(":/images/gamewon.png");
    leftclick=new QIcon(":/images/leftclick.png");
    new_game=new QIcon(":/images/newgame.png");
    tile=new QIcon(":/images/tile.png");
    tile_mine=new QIcon(":/images/tile_mine.png");
    tile_mine_flag=new QIcon(":/images/tile_mine_flag.png");
    tile_mine_nomine=new QIcon(":/images/tile_mine_nomine.png");
    tile_flagged=new QIcon(":/images/tile_flagged.png");
    tile_unsure=new QIcon(":/images/tile_unsure.png");
    redmine_broken=new QPixmap(":/images/redmine_broken.png");

    for(int i=0; i<18; i++)
    {
        tiles[i][0].coverstatus=-1;
        tiles[i][17].coverstatus=-1;

    }
    for(int j=1; j<17; j++)
    {
        tiles[0][j].coverstatus=-1;
        tiles[17][j].coverstatus=-1;
    }

    connect(timer, SIGNAL(timeout()), this, SLOT(timer_tick()));
    QFont info_font("Microsoft Sans Serif", 18);
    ui->minesleftvalue->setFont(info_font);
    ui->timepastvalue->setFont(info_font);
    ui->timepastvalue->setText(QString::number(time));
    QFont value_font("Consolas", 20, QFont::Bold);
    QGridLayout *grid = new QGridLayout;
    grid->setMargin(0);
    grid->setHorizontalSpacing(0);
    grid->setVerticalSpacing(0);
    int l=0, t=0;

    for(int i=1; i<17; i++)
    {
        grid->setRowMinimumHeight(i, 32);
        grid->setColumnMinimumWidth(i, 32);
        for(int j=1; j<17; j++)
        {
            tiles[i][j].value = new QLabel;
            tiles[i][j].value->setGeometry(l, t, 32, 32);
            tiles[i][j].value->setAlignment(Qt::AlignCenter);
            tiles[i][j].value->setFont(value_font);
            tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid;}");
            grid->addWidget(tiles[i][j].value, i, j);

            tiles[i][j].cover = new MyPushButton;
            tiles[i][j].cover->setGeometry(l, t, 32, 32);
            tiles[i][j].cover->setIconSize(QSize(32, 32));
            tiles[i][j].cover->setIcon(*tile);
            tiles[i][j].cover->setStyleSheet("MyPushButton {background-image: url(:/images/tile.png);}");
            connect(tiles[i][j].cover,SIGNAL(pressed(QMouseEvent *)), SLOT(tile_MouseDown(QMouseEvent *)));
            connect(tiles[i][j].cover,SIGNAL(released(QMouseEvent *)), SLOT(tile_MouseUp(QMouseEvent *)));
            grid->addWidget(tiles[i][j].cover, i, j);

            l+=32;
        }
        l=0;
        t+=32;
    }

    ui->field->setLayout(grid);
}

// Populates the game field with mines
void Widget::create_new_mines()
{
    int m=mines, x, y;
    qsrand(QTime::currentTime().msec());

    while(m>0)
    {
        x = qrand()%16+1;
        y = qrand()%16+1;
        if(tiles[x][y].valuestatus==0)
        {
            tiles[x][y].valuestatus=-1;
            m--;
        }
    }

    for(int i=1; i<17; i++)
    {
        for(int j=1; j<17; j++)
        {
            if(tiles[i][j].valuestatus!=-1)
            {
                tiles[i][j].valuestatus=0;
                if(tiles[i-1][j-1].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i-1][j].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i-1][j+1].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i][j-1].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i][j+1].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i+1][j-1].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i+1][j].valuestatus==-1) tiles[i][j].valuestatus++;
                if(tiles[i+1][j+1].valuestatus==-1) tiles[i][j].valuestatus++;

                if(tiles[i][j].valuestatus>0)
                {
                    tiles[i][j].value->setText(QString::number(tiles[i][j].valuestatus));
                    switch(tiles[i][j].valuestatus)
                    {
                    case 1:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : blue;}");
                        break;
                    case 2:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : green;}");
                        break;
                    case 3:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : red;}");
                        break;
                    case 4:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : purple;}");
                        break;
                    case 5:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : brown;}");
                        break;
                    case 6:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : aqua;}");
                        break;
                    case 7:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : black;}");
                        break;
                    case 8:
                        tiles[i][j].value->setStyleSheet("QLabel {border : 1px solid; color : gray;}");
                        break;
                    }
                }
            }
        }
    }
}

// Executes events when a tile is pressed
void Widget::tile_MouseDown(QMouseEvent *e)
{
    MyPushButton *clickedtile=(MyPushButton *)sender();
    int x=clickedtile->geometry().center().ry()/32+1;
    int y=clickedtile->geometry().center().rx()/32+1;
    if(e->button()==Qt::LeftButton)
    {
        if(tiles[x][y].coverstatus!=1)
        {
            ui->newgame->setIcon(*leftclick);

            if(timer->isActive()==false)
            {
                tiles[x][y].valuestatus=9;
                create_new_mines();
            }
        }
    }
    else if(e->button()==Qt::RightButton)
    {
        if(tiles[x][y].coverstatus==0)
        {
            if(mines>0)
            {
                tiles[x][y].cover->setIcon(*tile_flagged);
                tiles[x][y].coverstatus=1;

                mines--;
                ui->minesleftvalue->setText(QString::number(mines));
            }
        }
        else if(tiles[x][y].coverstatus==1)
        {
            tiles[x][y].cover->setIcon(*tile_unsure);
            tiles[x][y].coverstatus=2;

            mines++;
            ui->minesleftvalue->setText(QString::number(mines));
        }
        else
        {
            tiles[x][y].cover->setIcon(*tile);
            tiles[x][y].coverstatus=0;
        }
    }
}

// Executes events when a tile is released
void Widget::tile_MouseUp(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        MyPushButton *clickedtile=(MyPushButton *)sender();
        int x=clickedtile->geometry().center().ry()/32+1;
        int y=clickedtile->geometry().center().rx()/32+1;

        if(tiles[x][y].coverstatus!=1)
        {
            if(timer->isActive()==false) timer->start(1000);
            delete tiles[x][y].cover;
            tiles[x][y].coverstatus=-1;

            if(tiles[x][y].valuestatus==-1) game_lost(x, y);
            else
            {
                ui->newgame->setIcon(*new_game);
                closedsafetiles--;
                if(tiles[x][y].valuestatus==0) open_tile(x, y);
                if(closedsafetiles<=0) game_won();
            }
        }
    }
}

// Prompts the user to create a new game
void Widget::on_newgame_clicked()
{
    if(QMessageBox::question(this, "Minesweeper", "Do you really want to start a new game?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

// Updates the time past value
void Widget::timer_tick()
{
    ui->timepastvalue->setText(QString::number(++time));
}

// Executes if a game is lost
void Widget::game_lost(int x, int y)
{
    ui->newgame->setIcon(*gamelost);
    timer->stop();
    tiles[x][y].value->setPixmap(*redmine_broken);

    for(int i=1; i<17; i++)
    {
        for(int j=1; j<17; j++)
        {
            if(tiles[i][j].coverstatus!=-1)
            {
                if(tiles[i][j].valuestatus==-1)
                {
                    tiles[i][j].cover->setIcon(*tile_mine);
                    if (tiles[i][j].coverstatus==1) tiles[i][j].cover->setIcon(*tile_mine_flag);
                }
                else
                {
                    if(tiles[i][j].coverstatus==1) tiles[i][j].cover->setIcon(*tile_mine_nomine);
                }
            }
        }
    }

    if(QMessageBox::critical(this, "Game Lost", "Sorry, you lost this game. Do you want to start a new one?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
    exit(0);
}

// Executes if a game is won
void Widget::game_won()
{
    ui->newgame->setIcon(*gamewon);
    timer->stop();

    mines=0;
    ui->minesleftvalue->setText(QString::number(mines));

    for(int i=1; i<17; i++)
    {
        for(int j=1; j<17; j++)
        {
            if(tiles[i][j].coverstatus!=-1 && tiles[i][j].coverstatus!=1) tiles[i][j].cover->setIcon(*tile_mine);
        }
    }

    if(QMessageBox::information(this, "Game Won", "Congratulations, you won the game!. Do you want to start a new one?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
    exit(0);
}

// Opens all empty tiles and the first non-mine tiles, around a clicked empty tile
void Widget::open_tile(int x, int y)
{
    if(tiles[x-1][y-1].coverstatus!=-1)
    {
        delete tiles[x-1][y-1].cover;
        tiles[x-1][y-1].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x-1][y-1].valuestatus==0) open_tile(x-1, y-1);
    }

    if(tiles[x-1][y].coverstatus!=-1)
    {
        delete tiles[x-1][y].cover;
        tiles[x-1][y].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x-1][y].valuestatus==0) open_tile(x-1, y);
    }

    if(tiles[x-1][y+1].coverstatus!=-1)
    {
        delete tiles[x-1][y+1].cover;
        tiles[x-1][y+1].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x-1][y+1].valuestatus==0) open_tile(x-1, y+1);
    }

    if(tiles[x][y-1].coverstatus!=-1)
    {
        delete tiles[x][y-1].cover;
        tiles[x][y-1].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x][y-1].valuestatus==0) open_tile(x, y-1);
    }
    if(tiles[x][y+1].coverstatus!=-1)
    {
        delete tiles[x][y+1].cover;
        tiles[x][y+1].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x][y+1].valuestatus==0) open_tile(x, y+1);
    }

    if(tiles[x+1][y-1].coverstatus!=-1)
    {
        delete tiles[x+1][y-1].cover;
        tiles[x+1][y-1].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x+1][y-1].valuestatus==0) open_tile(x+1, y-1);
    }
    if(tiles[x+1][y].coverstatus!=-1)
    {
        delete tiles[x+1][y].cover;
        tiles[x+1][y].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x+1][y].valuestatus==0) open_tile(x+1, y);
    }

    if(tiles[x+1][y+1].coverstatus!=-1)
    {
        delete tiles[x+1][y+1].cover;
        tiles[x+1][y+1].coverstatus=-1;
        closedsafetiles--;
        if(tiles[x+1][y+1].valuestatus==0) open_tile(x+1, y+1);
    }
}

void Widget::choose_difficulty()
{
    difficultywindow difficulty_window;
    if (difficulty_window.exec()==QDialog::Accepted)
    {
        mines=difficulty_window.level();
        ui->minesleftvalue->setText(QString::number(mines));
        closedsafetiles-=mines;
    }
    else exit(0);
}
