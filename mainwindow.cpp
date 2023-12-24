#include "mainwindow.h"
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <QGridLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    camView = new QLabel(centralWidget);
    camView->setMinimumSize(640, 480);

    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    mainLayout->addWidget(camView, 0, 0, 1, 1);


    capture.open(0);
        if (!capture.isOpened())
        {
            qWarning("Failed to open webcam!");
        }

     QTimer *timer = new QTimer(this);
     connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
     timer->start(5); // FPS
}

MainWindow::~MainWindow()
{
    capture.release();
}

void MainWindow::updateFrame()
{
    cv::Mat frame;
    capture >> frame;

    if (!frame.empty())
    {
        cv::Mat hsvImage;
        cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

        determineLEDColor(hsvImage, frame);

        QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        qImage = qImage.rgbSwapped();

        camView->setPixmap(QPixmap::fromImage(qImage).scaled(camView->size(), Qt::KeepAspectRatio));
    }
}

void MainWindow::determineLEDColor(const cv::Mat& hsv, cv::Mat& frame) {
    // Определение диапазонов цветов для красного, синего и зеленого цветов
    cv::Scalar lower_red(0, 120, 70);
    cv::Scalar upper_red(10, 255, 255);
    cv::Scalar lower_blue(94, 80, 2);
    cv::Scalar upper_blue(126, 255, 255);
    cv::Scalar lower_green(36, 25, 25);
    cv::Scalar upper_green(86, 255,255);

    cv::Mat mask_red, mask_blue, mask_green;
    cv::inRange(hsv, lower_red, upper_red, mask_red);
    cv::inRange(hsv, lower_blue, upper_blue, mask_blue);
    cv::inRange(hsv, lower_green, upper_green, mask_green);

    // Проверка наличия красного, синего и зеленого цветов на изображении
        if (cv::countNonZero(mask_red) > 0 && cv::mean(hsv, mask_red)[2] > 100) {
            drawCircleAroundLED(mask_red, frame, "Red");
        } else if (cv::countNonZero(mask_blue) > 0 && cv::mean(hsv, mask_blue)[0] > 100) {
            drawCircleAroundLED(mask_blue, frame, "Blue");
        } else if (cv::countNonZero(mask_green) > 0 && cv::mean(hsv, mask_green)[1] > 100) {
            drawCircleAroundLED(mask_green, frame, "Green");
        }
}
void MainWindow::drawCircleAroundLED(const cv::Mat& mask, cv::Mat& frame, const std::string& colorString) {
    // Находим контур светодиода
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Фильтруем контуры по размеру
    const int minContourArea = 100;
    std::vector<std::vector<cv::Point>> filteredContours;
    for (const auto& contour : contours) {
        double contourArea = cv::contourArea(contour);
        if (contourArea > minContourArea) {
            filteredContours.push_back(contour);
        }
    }

    // Если есть фильтрованные контуры, найдем ограничивающий прямоугольник
    if (!filteredContours.empty()) {
        cv::Rect boundingRect;
        for (const auto& contour : filteredContours) {
            boundingRect |= cv::boundingRect(contour);
        }

        // Вычисляем центр и радиус круга на основе ограничивающего прямоугольника
        cv::Point center(boundingRect.x + boundingRect.width / 2, boundingRect.y + boundingRect.height / 2);
        int radius = std::max(boundingRect.width, boundingRect.height) / 2;

        cv::circle(frame, center, radius, cv::Scalar(255, 255, 255), 2);

        int baseline = 0;
        cv::Size textSize = cv::getTextSize(colorString, cv::FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseline);
        cv::putText(frame, colorString, cv::Point(center.x - textSize.width / 2, center.y - radius - baseline - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
    }
}
