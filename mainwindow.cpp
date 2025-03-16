#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QRect>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("人脸识别智能考勤系统");
    ui->success->hide();

    // 打开摄像头
    m_cap.open(0); // 0 : 使用笔记本摄像头, 1 : 使用 iPhone的摄像头

    // 启动定时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCamera);
    timer->start(100);

    // 导入 级联分类器 文件
    const std::string path = "/Users/bssn/Downloads/FaceRecognition/opencv_452/share/opencv4/haarcascades/haarcascade_frontalface_alt2.xml";
    m_cascade.load(path);

    // 初始化套接字
    connect(&m_socket, &QTcpSocket::disconnected, this, &MainWindow::start_connect);
    connect(&m_socket, &QTcpSocket::connected, this, &MainWindow::stop_connect);
    connect(&m_socket, &QTcpSocket::readyRead, this, &MainWindow::recvFaceData);

    // 定时连接服务器
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::timer_connect);
    // 启动定时器，自动重连：每 5s 尝试连接一次服务端，直至连接成功
    m_timer.start(5000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#if 0 // 加速版本

void MainWindow::updateCamera()
{
    Mat srcImage;
    if (m_cap.grab()) {
        m_cap.retrieve(srcImage);
    }
    if (srcImage.empty()) return;

           // 1. 水平翻转图像
    cv::flip(srcImage, srcImage, 1);

           // 2. 灰度转换（但不每帧都检测人脸）
    Mat grayImage;
    cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);

           // 3. 每 5 帧检测一次人脸
    static int frameCount = 0;
    frameCount++;
    static std::vector<Rect> faceRects; // 存储最近一次检测到的人脸
    if (frameCount % 3 == 0)
    {
        m_cascade.detectMultiScale(grayImage, faceRects, 1.1, 3, 0, cv::Size(50, 50));
    }
            // 4. 显示矩形框
    if (faceRects.size() > 0)
    {
        Rect rect = faceRects.at(0); // 第一个人脸的矩形框
        rectangle(srcImage, rect, Scalar(0,0,255, 2));

               // 移动人脸框
        // QRect rectTest(10, 10, 10, 10);
        ui->circle->move(rect.x, rect.y);
    }

           // 5. 转换格式并显示
    cv::cvtColor(srcImage, srcImage, cv::COLOR_BGR2RGB);
    QImage image(srcImage.data, srcImage.cols, srcImage.rows, srcImage.step, QImage::Format_RGB888);
    ui->camera->setPixmap(QPixmap::fromImage(image).scaled(ui->camera->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

#elif 1 // 人脸局中显示并填充整个QLabel
void MainWindow::updateCamera()
{
    // 1. 采集数据
    Mat srcImage;
    if(m_cap.grab())
    {
        m_cap.read(srcImage); // 读取一帧数据
    }
    if (srcImage.data == nullptr) return;

    // 2. 为采集的图像加上 人脸矩形框
    faceFrame(srcImage);

    // 3. 水平翻转图像
    cv::flip(srcImage, srcImage, 1); // 1表示水平翻转

    // 4. 图片格式转换 ：把opencv里面的Mat格式数据(BGR)转化为QT里面的QImage(RGB)
    cv::cvtColor(srcImage, srcImage, cv::COLOR_BGR2RGB);
    QImage image(srcImage.data, srcImage.cols, srcImage.rows, srcImage.step1(), QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(image);

    // 5. 显示图像到 QLabel
    /* KeepAspectRatio	完整显示图像，留黑边	需全览画
     * KeepAspectRatioByExpanding	填充区域，裁剪边缘	突出人脸区域
     * IgnoreAspectRatio	拉伸变形	不推荐
     */
    if (ui->camera)
    {
        ui->camera->setPixmap(pix.scaled(ui->camera->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}

#endif


void MainWindow::faceFrame(Mat &srcImage)
{
    Mat grayImage;
    cvtColor(srcImage, grayImage, COLOR_BGR2GRAY); // 将采集到的图像转为灰度图从而加速检测

    // 检测人脸数据
    std::vector<Rect> faceRects;
    m_cascade.detectMultiScale(grayImage, faceRects, 1.2, 3, 0, cv::Size(30, 30));

    if (faceRects.size() > 0 && m_faceCheckFlag >= 0) // 如果成功检测到人脸数据
    {
        Rect rect = faceRects.at(0);
        // rectangle(srcImage, rect, Scalar(0,0,255), 2);

        // 控制发送次数
        if (m_faceCheckFlag > 2)
        {
            // 将数据发送到服务器
            sendFaceData(srcImage);

            m_faceCheckFlag = -2;

            m_faceMat = srcImage(rect);  // 保存矩形框内的人脸图像
            cv::imwrite("./face.jpg", m_faceMat);
        }
        m_faceCheckFlag++;

        // 坐标镜像与中心计算
        int flippedX = srcImage.cols - rect.x - rect.width;
        float scaleX = (float)ui->camera->width() / srcImage.cols;
        float scaleY = (float)ui->camera->height() / srcImage.rows;

        // 检测框中心映射到显示坐标系
        int centerX = (flippedX + rect.width/2) * scaleX;
        int centerY = (rect.y + rect.height/2) * scaleY;

        // 边界保护
        centerX = qBound(ui->circle->width()/2, centerX, ui->camera->width() - ui->circle->width()/2);
        centerY = qBound(ui->circle->height()/2, centerY, ui->camera->height() - ui->circle->height()/2);

        // 控件中心对齐
        ui->circle->move(centerX - ui->circle->width()/2,
                         centerY - ui->circle->height()/2);
    }

    if (faceRects.size() == 0)    // 如果没有检测到人脸
    {
        // 移动人脸框
        ui->circle->move(100, 60);
        // 清除标识
        m_faceCheckFlag = 0;
        // 隐藏结果框
        ui->success->hide();
    }
}

void MainWindow::sendFaceData(const Mat &srcImage)
{
    if (m_socket.state() != QTcpSocket::ConnectedState)
        return;

    // 1. 将 Mat 格式转化为 QByteArray --> 编码成 jpg 格式
    std::vector<uchar> buf;
    cv::imencode(".jpg", srcImage, buf);
    QByteArray byte((const char*)buf.data(), buf.size());

    // 2. 准备发送
    quint64 backSize = byte.size();
    QByteArray sendData;
    QDataStream stream(&sendData, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);
    stream << backSize << byte;

    // 3. 发送
    m_socket.write(sendData);
    qDebug() << "***** 发送成功 *****";

    // 4. 显示头像
    // 通过样式来显示圆形图片
    ui->avatar->setStyleSheet("border-radius: 80px;border-image: url(./face.jpg)");
}

void MainWindow::recvFaceData()
{
    QByteArray faceData = m_socket.readAll();

    // Json 解析
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(faceData, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "json数据错误";
        return;
    }

    qDebug() << "数据接收: " << faceData;

    QJsonObject obj = doc.object();
    QString employeeID = obj.value("employeeID").toString();
    QString name = obj.value("name").toString();
    QString department = obj.value("department").toString();
    QString date = obj.value("date").toString();

    ui->employeeIDEdit->setText(employeeID);
    ui->nameEdit->setText(name);
    ui->departmentEdit->setText(department);
    ui->dateEdit->setText(date);

    // 展示 认证成功 弹窗
    ui->success->show();

}


void MainWindow::timer_connect()
{
    // 连接服务器
    m_socket.connectToHost("127.0.0.1", 9999);
    qDebug() << "尝试重连中...\n";
}

void MainWindow::stop_connect()
{
    m_timer.stop();
    qDebug() << "连接成功\n";
}

void MainWindow::start_connect()
{
    m_timer.start(5000);
    qDebug() << "服务端断开连接\n";
}
