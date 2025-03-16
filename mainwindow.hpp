#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimerEvent>
#include <QTcpSocket>
#include <QTimer>

#include <opencv.hpp>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 1. 图像的采集与显示
    void updateCamera();

    // 2. 人脸数据的识别
    void faceFrame(Mat &srcImage);

    // 3. 将人脸数据发送到服务端
    void sendFaceData(const Mat &srcImage);

    // 4. 接收服务端发送的人脸数据
    void recvFaceData();

private slots:
    void timer_connect(); // 当定时器启动后，每隔 5s 尝试连接一次
    void stop_connect(); // 当连接成功时，停止定时器
    void start_connect(); // 当断开连接时，启动定时器

private:
    Ui::MainWindow *ui;

    // 摄像头
    VideoCapture m_cap;
    // haar 级联分类器
    cv::CascadeClassifier m_cascade;
    // 套接字，定时器
    QTcpSocket m_socket;
    QTimer m_timer;
    // 用于标识是否是同一个人脸进入到识别区域，避免资源浪费
    int m_faceCheckFlag = 0;
    // 保存服务器回传的人脸图像
    cv::Mat m_faceMat;
};
#endif // MAINWINDOW_HPP
