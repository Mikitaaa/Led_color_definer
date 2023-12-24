#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    cv::VideoCapture capture;

    QLabel* camView;
    void determineLEDColor(const cv::Mat& hsv, cv::Mat& frame);
    void drawCircleAroundLED(const cv::Mat& mask, cv::Mat& frame,  const std::string& colorString);
private slots:
    void updateFrame();
};
#endif // MAINWINDOW_H
