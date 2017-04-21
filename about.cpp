#include "about.h"
#include "ui_about.h"
#include <QPixmap>


About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    QPixmap image;
    image.load(":/new/Icons/clock.png");

    ui->lblIcon->setPixmap(image);

    QString aboutText;
    aboutText.append("WeatherAlarm created by DybonSoft");
    aboutText.append("\n");
    aboutText.append("\n");
    aboutText.append("\n");
    aboutText.append("Please report are issues and bugs");
    aboutText.append("\n");
    aboutText.append("feature request to my email or github");

    ui->lblAbout->setText(aboutText);
    ui->lblLink->setOpenExternalLinks(true);

    QString textLink;
    textLink.append("<a href=mailto:dybonek@gmail.com?Subject=Hello%20Dybon> Send Mail</a>");
    textLink.append("\n");
    textLink.append("\n");
    //put github reference also
    // textLink.append("<a href=https://github.com/TheDybon");
    ui->lblLink->setText(textLink);



}

About::~About()
{
    delete ui;
}
