#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "pixel.hpp"

pixel::PixelSet genPixelSet(const cv::Mat &src)
{
  constexpr double epsCoeff = 0.1;
  constexpr double deltaT = 0.1;

  cv::Mat gradX, gradY;
  cv::Sobel(src, gradX, CV_64F, 1, 0, 3);
  cv::Sobel(src, gradY, CV_64F, 0, 1, 3);

  cv::Mat gradMag;
  cv::magnitude(gradX, gradY, gradMag);
  double gradMax;
  cv::minMaxLoc(gradMag, NULL, &gradMax, NULL, NULL);

  const auto eps = gradMax * epsCoeff;
  std::vector<cv::Point> movingIndexes;
  cv::findNonZero(gradMag > eps, movingIndexes);

  std::vector<pixel::Pixel> pixels;
  for (const auto &index : movingIndexes)
    pixels.push_back(
        pixel::Pixel(
            pixel::Vector(index.x, index.y),
            pixel::Vector(gradX.at<double>(index), gradY.at<double>(index)) * deltaT));

  return pixel::PixelSet(pixels);
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "no file specified" << std::endl;
    return 1;
  }
  cv::Mat raw = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if (raw.empty()) {
    std::cerr << "can't open file '" << argv[1] << "'" << std::endl;
    return 1;
  }
  cv::Mat src;
  raw.convertTo(src, CV_64F, 1.0 / 256);

  auto pixelSet = genPixelSet(src);
  const auto active = pixelSet.countActives();

  while (pixelSet.countActives() > active * 0.1) {
    pixelSet.move();
    std::cout << pixelSet.countActives() << std::endl;
  }

  cv::imshow("input", src);

  while (cv::waitKey(0) != 'q') {
  };

  return 0;
}
