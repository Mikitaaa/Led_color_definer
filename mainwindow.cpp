#include "mainwindow.h"
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <QGridLayout>

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
     timer->start(33); // 33 FPS
}

void MainWindow::updateFrame()
{
    cv::Mat frame;
    capture >> frame; // Захватываем кадр с вебкамеры

    if (!frame.empty())
    {
        QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        qImage = qImage.rgbSwapped(); // Переключаем каналы RGB

        camView->setPixmap(QPixmap::fromImage(qImage).scaled(camView->size(), Qt::KeepAspectRatio));
    }
}

MainWindow::~MainWindow()
{
    capture.release();
}
