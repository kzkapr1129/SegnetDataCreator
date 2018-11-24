//
//  CanvasWindow.hpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#pragma once

#include <opencv2/opencv.hpp>

typedef void (*OnSaveFunc)(void* userdata, const cv::Mat& srcImg, const cv::Mat& labelImg);

class CanvasWindow {
public:
    CanvasWindow();
    
    void setCallback(OnSaveFunc saveFunc, void* userdata);
    void setImage(const cv::Mat& img);
    void setColor(const cv::Scalar& cc, const cv::Scalar& lc);
    
    void clear();
    void clearVertexes();
    
    bool isNeededImage();
    
private:
    void updateLabelWindow();
    void release();
    static void onMouseEvent(int eventType, int x, int y, int flags, void* userdata);
    
    cv::Mat mSrcImg;
    cv::Mat mCommitedImage;
    cv::Mat mWorkImg;
    cv::Mat mLabelImg;
    cv::Scalar mClassColor;
    cv::Scalar mLabelColor;
    std::vector<cv::Point> mVertexes;
    OnSaveFunc mOnSaveFunc;
    void* mUserdata;
};
