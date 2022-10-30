#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QWidget"
#include "QImage"
#include "QPixmap"
#include "QFileDialog"
#include "QPainter"
#include "QRgb"
#include <cmath>
#include <iostream>
#include <vector>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString str;

void MainWindow::on_open_image_clicked()
{
    str = QFileDialog::getOpenFileName(0, "Open Dialog", "", "* .png * .jpg * .bmp"); // получение пути к изображению
    QPixmap img(str); //получение изображения

    img = img.scaled(ui->image->width(),
                     ui->image->height(),
                     Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);  //функция масштаба

    ui->image->setPixmap(img); // вывод начального изображения
}

QImage MainWindow::capture_image(const QPixmap* img_pix){ //  ui->image->pixmap()
    // Проверка на наличие загруженного изображения
    if ( img_pix != nullptr ) {
        const QPixmap* pix = img_pix;
        return QImage(pix->toImage());
    }
    return QImage(":/img/no_image.jpg").scaled(ui->image_2->size());
}

void MainWindow::set_image(QImage &img){
    QPixmap pix = QPixmap::fromImage(img);
    ui->image_2->setPixmap(pix);
}

bool MainWindow::in_color_range(int val){
    if (val < 256 && val >=0) return true;
    return false;
}

void MainWindow::process_image(QImage &img, const std::string color){
    QColor rgb;

    if(color == "red") {
        for (int i = 0; i < img.width(); ++i) {
            for (int j = 0; j < img.height(); ++j) {
                rgb = img.pixel(i,j);
                rgb.setGreen(0);
                rgb.setBlue(0);
                img.setPixelColor(i,j,rgb);
            }
        }
    } else if (color == "green") {
        for (int i = 0; i < img.width(); ++i) {
            for (int j = 0; j < img.height(); ++j) {
                rgb = img.pixel(i,j);
                rgb.setRed(0);
                rgb.setBlue(0);
                img.setPixelColor(i,j,rgb);
            }
        }
    } else if (color == "blue") {
        for (int i = 0; i < img.width(); ++i) {
            for (int j = 0; j < img.height(); ++j) {
                rgb = img.pixel(i,j);
                rgb.setRed(0);
                rgb.setGreen(0);
                img.setPixelColor(i,j,rgb);
            }
        }
    } else if (color == "grey") {
        float color_gray = 0;
        for (int i = 0; i < img.width(); ++i){
            for (int j = 0; j < img.height(); ++j){
                rgb = img.pixel(i,j);
                color_gray = rgb.red()*0.299 + rgb.green()*0.587 + rgb.blue()*0.114;
                rgb.setRed(color_gray);
                rgb.setGreen(color_gray);
                rgb.setBlue(color_gray);
                img.setPixelColor(i,j,rgb);
            }
        }
    }
}

void MainWindow::full_process_img(const std::string color) {
    QImage image = capture_image(ui->image_2->pixmap());
    process_image(image, color);
    set_image(image);
}

void MainWindow::on_red_ratio_pressed() {
    full_process_img("red");
}

void MainWindow::on_green_ratio_pressed() {
    full_process_img("green");
}


void MainWindow::on_blue_ratio_pressed() {
    full_process_img("blue");
}

void MainWindow::on_initial_image_clicked() {
    QImage image = capture_image(ui->image->pixmap());
    set_image(image);
}

void MainWindow::on_grey_image_clicked() {
    full_process_img("grey");
}

void MainWindow::on_apply_scale_clicked() {
    QImage image = capture_image(ui->image_2->pixmap());
    QPixmap pix = QPixmap::fromImage(image);

    double scale = std::stod(ui->scale->text().toStdString());
    QSize picture_size(image.width()*scale, image.height()*scale);

    pix = pix.scaled(picture_size, Qt::KeepAspectRatio);
    ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_angle_center_coord_clicked() {

}

void MainWindow::on_apply_center_img_clicked() {
    QImage image = capture_image(ui->image_2->pixmap());
    QPixmap pix = QPixmap::fromImage(image);
    QPixmap rotate(pix.size());
    QPainter p(&rotate);
    rotate.fill(Qt::transparent);

    double angle = std::stod(ui->angle_center_coord->text().toStdString());
    p.translate(rotate.size().width() / 2, rotate.size().height() / 2);
    p.rotate(angle); // градус
    p.translate(-rotate.size().width() / 2, -rotate.size().height() / 2);
    p.drawPixmap(0, 0, pix);
    p.end();
    pix = rotate;
    ui->image_2->clear();
    ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_mirror_horizontal_clicked() {

}

void MainWindow::on_apply_mirror_vertical_clicked() {

}

void MainWindow::on_apply_angle_bevel_clicked() {

}

