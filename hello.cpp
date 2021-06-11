#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <opencv2/cvconfig.h>
//#include <opencv2/opencv_modules.hpp>


#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>




#include <stdio.h>
#include <string>
#include <iostream>
#pragma comment(lib, "opencv_core2413" CV_EXT)
#pragma comment(lib, "opencv_imgproc2413" CV_EXT)
#pragma comment(lib, "opencv_highgui2413" CV_EXT)
using namespace std;
using namespace cv;
using namespace emscripten;

class Hello {
private:
  std::vector<unsigned char> buffer;
  unsigned char * buffer2;
  Mat decoded;

public:
  Hello() : buffer{}, decoded{} {};

  val allocate(size_t size) {
    this->buffer.reserve(size);
    unsigned char *byteBuffer = this->buffer.data();
    return val(typed_memory_view(size, byteBuffer));
  }

  val my_imdecode(uintptr_t addr, int size, int width, int height) {
    this->buffer2 = new unsigned char[size];
    printf("%d\n",size);
    printf("%d\n",width);
    printf("%d\n",height);
    // 引数の配列をコピー
    uint8_t * data = reinterpret_cast<uint8_t *>(addr);
    for (int i = 0 ; i!= size; i++){
      buffer2[i] = data[i];
    }

    // Mat型へ変換
    printf("here convert to Mat.\n");
    // Mat image = Mat(size, 1, CV_8U, buffer2).clone();
    // Mat image = Mat(height, width, CV_8U, buffer2);
    // Mat image = Mat(height, width, CV_8U, data);
    Mat image = Mat(height, width, CV_32FC1, data);
    // vector<uchar> data2 = vector<uchar>(buffer2, buffer2 + size);
    // buf.assign(buffer2, buffer2 + size);
    // Mat image = imdecode(buffer2, CV_LOAD_IMAGE_ANYDEPTH);

    // アフィン変換処理
    printf("here start affine trns 1.\n");
    cv::Mat dstImg;
    //image.copyTo(dstImg);
    // cvtColor(image, dstImg, COLOR_RGB2GRAY);
    // Mat mat = (cv::Mat_<double>(2,3)<<100.0, 100.0, 100, 1.0, 100.0, 100);
    // warpAffine(image, dstImg, mat, dstImg.size(), INTER_LINEAR, cv::BORDER_TRANSPARENT);
    cv::Point2f center = cv::Point2f(
        static_cast<float>(image.cols / 2),
        static_cast<float>(image.rows / 2));
    printf("here start affine trns 2.\n");
    double degree = 45.0;  // 回転角度
    double scale = 1.0;    // 拡大率
 
    // アフィン変換行列
    cv::Mat affine;
    printf("here start affine trns 3.\n");    
    cv::getRotationMatrix2D(center, degree, scale).copyTo(affine);
    printf("here start affine trns 4.\n");
    // cv::warpAffine(image, image, affine, image.size());
    cv::flip(image, dstImg, 0); // 垂直軸で反転（水平反転
    printf("here start affine trns 5.\n");
    /*
    cv::Mat srcImg = cv::imread("lenna.jpg");
    if (srcImg.empty())
        return 1;
 
    cv::Point2f center = cv::Point2f(
        static_cast<float>(srcImg.cols / 2),
        static_cast<float>(srcImg.rows / 2));
 
    double degree = 45.0;  // 回転角度
    double scale = 1.0;    // 拡大率
 
    // アフィン変換行列
    cv::Mat affine;
    cv::getRotationMatrix2D(center, degree, scale).copyTo(affine);
 
    cv::warpAffine(srcImg, srcImg, affine, srcImg.size(), cv::INTER_CUBIC);
 
    cv::namedWindow("image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
    cv::imshow("image", srcImg);
    cv::waitKey();
    */

    // コピーした配列を返却
    /*
    int sizeOfImage = image.total() * image.elemSize();
    unsigned char * bytes = new unsigned char[sizeOfImage];  // you will have to delete[] that later
    std::memcpy(bytes, image.data, sizeOfImage * sizeof(unsigned char));
    */
    // return val(typed_memory_view(size, buffer2));
    printf("here output image.\n");
    //imwrite("test.png", dstImg);
    //int sizeOfImage = dstImg.total() * dstImg.elemSize();
    //int sizeOfImage = dstImg.total();
    int sizeOfImage = size * sizeof(unsigned char);
    // cout << "M = " << endl << " "  << dstImg << endl << endl;
    printf("%zu\n", dstImg.total());
    printf("%zu\n", dstImg.elemSize());
    printf("%zu\n", sizeof(unsigned char));    
    unsigned char * bytes = new unsigned char[sizeOfImage];  // you will have to delete[] that later
    for(int i=0;i!=10;i++)
      printf("%d\n",dstImg.data[i]);
    //std::memcpy(bytes, dstImg.data, sizeOfImage);
    std::memcpy(bytes, dstImg.data, sizeOfImage);
    return val(typed_memory_view(sizeOfImage, bytes));
  }
};

EMSCRIPTEN_BINDINGS(my_module) {
  printf("Hello World!\n");
  class_<Mat>("Mat");
  class_<Hello>("Hello")
      .constructor<>()
      .function("imdecode", &Hello::my_imdecode, allow_raw_pointers())
      .function("allocate", &Hello::allocate);
}
