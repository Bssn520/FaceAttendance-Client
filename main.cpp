#include "mainwindow.hpp"

#include <QApplication>

#include <FaceDetector.h>
#include <opencv.hpp>

using namespace cv;
using namespace seeta::v2;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
