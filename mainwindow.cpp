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
#include <QtMath>

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

//  if ( ui->image_2 != nullptr ) {
//    this->previous_state.convertFromImage(ui->image_2)
//  }

  ui->image_2->setPixmap(pix);
}

bool MainWindow::in_color_range(int val){
  if (val < 256 && val >=0) return true;
  return false;
}

void MainWindow::image_scaling(QImage &img, QImage &new_img,
                               const double &scale) {
   for (int j = 0; j < img.height(); ++j) {
     for (int i = 0; i < img.width(); ++i) {
      if (i * scale < new_img.width() && j * scale < new_img.height()) {
        new_img.setPixel(i * scale,
                         j * scale,
                         img.pixel(i, j));
      }
    }
  }
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
  this->previous_state.push_back(image);
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
  this->previous_state.push_back(image);
  set_image(image);
}

void MainWindow::on_grey_image_clicked() {
  full_process_img("grey");
}



void MainWindow::on_apply_scale_clicked() {
  QImage image = capture_image(ui->image_2->pixmap());
  this->previous_state.push_back(image);
  // получаем коэффициент масштабирования
  double scale = std::stod(ui->scale->text().toStdString());
  QImage new_image(image.width()*scale,
                   image.height()*scale,
                   QImage::Format_RGB32);;

  image_scaling(image, new_image, scale);

  QPixmap pix = QPixmap::fromImage(new_image);
  ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_angle_center_coord_clicked() {
  QImage image = capture_image(ui->image_2->pixmap());
  this->previous_state.push_back(image);
  QImage new_image(image.width(),
                   image.height(),
                   QImage::Format_RGB32);

  double angle = std::stod(ui->angle_center_coord->text().toStdString());
  // из градусов в радианы
  angle = qDegreesToRadians(angle);

  double a, b;
  for (int i = 0; i < image.width(); ++i) {
    for (int j = 0; j < image.height(); ++j) {
      a = i*cos(angle)-j*sin(angle);
      b = i*sin(angle)+j*cos(angle);
      if (i*cos(angle) < new_image.width() &&
          i*sin(angle) < new_image.width() &&
          j*sin(angle) < new_image.height() &&
          j*cos(angle) < new_image.height()) {
        new_image.setPixel(i, j, image.pixel(roundf(a),
                                             roundf(b))); // вне диапазона
      }

    }
  }

  QPixmap pix = QPixmap::fromImage(new_image);
  ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_center_img_clicked() {
  QImage image = capture_image(ui->image_2->pixmap());
  this->previous_state.push_back(image);
  QImage new_image(image.width(),
                   image.height(),
                   QImage::Format_RGB32);
  double angle = std::stod(ui->angle_center_img->text().toStdString());
  // из градусов в радианы
  angle = qDegreesToRadians(angle);

  double a, b;

  for (int j = 0; j < image.height(); ++j) {
    for (int i = 0; i < image.width(); ++i) {
      a = (i-image.width()/2)*cos(angle)-(j-image.height()/2)*sin(angle)+image.width()/2;
      b = (i-image.width()/2)*sin(angle)+(j-image.height()/2)*cos(angle)+image.height()/2;
      new_image.setPixel(i, j, image.pixel(roundf(a),
                                           roundf(b))); // вне диапазона
    }
  }

  QPixmap pix = QPixmap::fromImage(new_image);
  ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_mirror_horizontal_clicked() {
  QImage image = capture_image(ui->image_2->pixmap());
  this->previous_state.push_back(image);
  QImage new_image(image.width(),
                   image.height(),
                   QImage::Format_RGB32);

  for (int i = 0; i < new_image.width(); ++i) {
    for (int j = 0; j < new_image.height(); ++j) {
      new_image.setPixel(i, j, image.pixel(i, image.height() - j - 1)); // вне диапазона
    }
  }

  QPixmap pix = QPixmap::fromImage(new_image);
  ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_mirror_vertical_clicked() {
  QImage image = capture_image(ui->image_2->pixmap());
  this->previous_state.push_back(image);
  QImage new_image(image.width(),
                   image.height(),
                   QImage::Format_RGB32);

  for (int i = 0; i < new_image.width(); ++i) {
    for (int j = 0; j < new_image.height(); ++j) {
      new_image.setPixel(i, j, image.pixel(image.width() - i - 1, j)); // вне диапазона
    }
  }

  QPixmap pix = QPixmap::fromImage(new_image);
  ui->image_2->setPixmap(pix);
}

void MainWindow::on_apply_angle_bevel_clicked() {
  QImage image = capture_image(ui->image_2->pixmap());
  this->previous_state.push_back(image);
  QImage new_image(image.width(),
                   image.height(),
                   QImage::Format_RGB32);
  double koeff_A = std::stod(ui->koeff_A->text().toStdString());
  double koeff_B = std::stod(ui->koeff_B->text().toStdString());
  double koeff_C = std::stod(ui->koeff_C->text().toStdString());
  double koeff_D = std::stod(ui->koeff_D->text().toStdString());
  int min_size;

  if (new_image.width() > new_image.height()) {
      min_size = new_image.height();
  } else {
      min_size = new_image.width();
  }

  for (int i = 0; i < min_size; ++i) {
    for (int j = 0; j < min_size; ++j) {
      new_image.setPixel(i, j, image.pixel(koeff_A * i + koeff_B * j,
                                           koeff_C * i + koeff_D * j)); // вне диапазона
    }
  }

  QPixmap pix = QPixmap::fromImage(new_image);
  ui->image_2->setPixmap(pix);
}


void MainWindow::on_revoke_clicked() {
  if (this->previous_state.size() > 0) {
    QImage image = this->previous_state[this->previous_state.size()-1];
    this->previous_state.pop_back();
    QPixmap pix = QPixmap::fromImage(image);
    ui->image_2->setPixmap(pix);
  }
}

