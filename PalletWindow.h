//
//  PalletWindow.hpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#pragma once

#include <opencv2/opencv.hpp>

typedef void (*OnClassChangedFunc) (void* userdata);

class PalletWindow {
public:
    PalletWindow();
    
    void setCallback(OnClassChangedFunc func, void* userdata);
    
    const cv::Scalar& getFocusedClassColor() const;
    const cv::Scalar& getFocusedClassLabel() const;
    
private:
    void draw();
    static void onPalletChanged(int eventType, int x, int y, int flags, void* userdata);
    
    int mFocusedClass;
    OnClassChangedFunc mCallbackFunc;
    void* mUserdata;
};
