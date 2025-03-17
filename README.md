# 人脸识别考勤系统 - 客户端

基于 **Qt5 + OpenCV + SeetaFace2** 实现的高效、稳定的人脸识别智能考勤系统，支持本地数据库阅览、人脸注册与识别等功能，适用于学校、公司等场景。

<div style="text-align: center;">
  <img src="https://github.com/Bssn520/picx-images-hosting/raw/master/20250317/example1.73u3s2sp8p.webp" alt="image" style="zoom:30%;" />
</div>

## 🚀 功能特点

-   📷 **人脸检测**：基于 SeetaFace2，实现高精度人脸检测
-   🏷️ **人脸注册**：支持录入人脸数据，生成唯一身份标识
-   🔍 **人脸识别**：快速匹配数据库中的人脸信息
-   📊 **考勤管理**：记录签到数据，支持查询和导出
-   🎨 **跨平台支持**：适用于 Windows、Linux、macOS、嵌入式设备

## 🔨 开发环境

-   Qt 5.15.16
-   [OpenCv 4.5.2](https://github.com/opencv/opencv/archive/4.5.2.zip)
-   [OpenCv Contrib 4.5.2](https://github.com/opencv/opencv_contrib/archive/refs/tags/4.5.2.zip)
-   [SeetaFace2](https://github.com/seetafaceengine/SeetaFace2)
-   Qt Creator 15.0.1
-   macOS 15.3.1

## 🔧 构建 & 运行

-   在构建前，请确保您已经安装了对应版本的 **OpenCV、SeetaFace2**;
-   克隆到本地: `git clone https://github.com/Bssn520/FaceAttendance-Client.git`
-   双击 `opencvSeetaFace.pro` 文件即可调用 Qt Creator 打开本项目，在 Qt Creator 内进行构建、测试即可。

## 🖥️ 使用指南

-   本仓库为 **人脸识别考勤系统 - 客户端**，需自行部署 **人脸识别考勤系统 - 服务端** 进行使用；
-   **人脸识别考勤系统 - 服务端**仓库链接: [https://github.com/Bssn520/FaceAttendance-Server](https://github.com/Bssn520/FaceAttendance-Server)

## ✨ Todo

-   [ ] 使用配置文件配置项目里的模型路径等；
-   [ ] 支持 CMake 对本项目进行构建；
-   [ ] 使用 FaceNet、InsightFace 等模型替代 SeetaFace2 模型；
-   [ ] ...
