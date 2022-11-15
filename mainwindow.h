#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Открывает изображение
    void on_open_image_clicked();

    // Вычленяет цветовой канал
    void on_red_ratio_pressed();
    void on_green_ratio_pressed();
    void on_blue_ratio_pressed();

    // Изменяет изображение по параметрам
    void on_initial_image_clicked();
    void on_grey_image_clicked();
    void on_apply_scale_clicked();
    void on_apply_angle_center_coord_clicked();
    void on_apply_center_img_clicked();
    void on_apply_mirror_horizontal_clicked();
    void on_apply_mirror_vertical_clicked();
    void on_apply_angle_bevel_clicked();
    void on_revoke_clicked();

private:
    Ui::MainWindow *ui;

    // для обработки изображения по трем цветовым каналам
    QImage capture_image(const QPixmap* img_pix);

    bool in_color_range(int val);

    void process_image(QImage &img, const std::string color);
    void set_image(QImage &img);
    void full_process_img(const std::string color);
    void image_scaling(QImage &img, QImage &new_img, const double &scale);

    std::vector<QImage> previous_state;


};
#endif // MAINWINDOW_H
