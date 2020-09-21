//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Utils.h
// @brief: Common fuction head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream> 
#include <iostream>
#include <math.h>
#include <windows.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

using namespace std;

void intDataSaveAsText(int* data, int height, int width, string TextPath);
void CharDataSaveAsText(unsigned char* data, int height, int width, string TextPath);