#pragma once

#include <vector>
#include <opencv2/core.hpp>

template <size_t N>
using Polygon_ = std::vector<cv::Vec<float, N>>; 