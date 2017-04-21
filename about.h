#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QPixMap>
#include <QDialog>


namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

private:
    Ui::About *ui;
};

#endif // ABOUT_H
