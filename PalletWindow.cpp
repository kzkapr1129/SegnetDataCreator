//
//  PalletWindow.cpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#include "PalletWindow.h"
#include <opencv2/opencv.hpp>

static const int PALLTE_WINDOW_WIDTH = 500;
static const int PALLTE_WINDOW_HEIGHT = PALLTE_WINDOW_WIDTH * 3/4;
static const int TILE_W = PALLTE_WINDOW_WIDTH / 4;
static const int TILE_H = PALLTE_WINDOW_HEIGHT / 3;
static const int NUMBER_MERGIN_Y = 30;
static const int FOCUSED_RECT_MERGIN = 3;
static const char* PALLET_WINDOW_NAME = "class pallet";

struct Class {
    cv::Rect rect;
    cv::Scalar color;
    cv::Scalar label;
    Class(const cv::Rect& r, const cv::Scalar& c, const cv::Scalar& l)
    : rect(r), color(c), label(l) {}
};

static const Class CLASSES[] = {
    Class(cv::Rect(       0,        0, TILE_W, TILE_H), cv::Scalar(  0,  0,  0), cv::Scalar(0)),
    Class(cv::Rect(  TILE_W,        0, TILE_W, TILE_H), cv::Scalar(128,  0,  0), cv::Scalar(1)),
    Class(cv::Rect(TILE_W*2,        0, TILE_W, TILE_H), cv::Scalar(255,  0,  0), cv::Scalar(2)),
    Class(cv::Rect(TILE_W*3,        0, TILE_W, TILE_H), cv::Scalar(  0,128,  0), cv::Scalar(3)),
    Class(cv::Rect(       0,   TILE_H, TILE_W, TILE_H), cv::Scalar(  0,255,  0), cv::Scalar(4)),
    Class(cv::Rect(  TILE_W,   TILE_H, TILE_W, TILE_H), cv::Scalar(  0,  0,128), cv::Scalar(5)),
    Class(cv::Rect(TILE_W*2,   TILE_H, TILE_W, TILE_H), cv::Scalar(  0,  0,200), cv::Scalar(6)),
    Class(cv::Rect(TILE_W*3,   TILE_H, TILE_W, TILE_H), cv::Scalar(128,128,  0), cv::Scalar(7)),
    Class(cv::Rect(       0, TILE_H*2, TILE_W, TILE_H), cv::Scalar(255,255,  0), cv::Scalar(8)),
    Class(cv::Rect(  TILE_W, TILE_H*2, TILE_W, TILE_H), cv::Scalar(  0,128,128), cv::Scalar(9)),
    Class(cv::Rect(TILE_W*2, TILE_H*2, TILE_W, TILE_H), cv::Scalar(  0,255,255), cv::Scalar(10)),
    Class(cv::Rect(TILE_W*3, TILE_H*2, TILE_W, TILE_H), cv::Scalar(128,  0,128), cv::Scalar(11))
};
static const size_t NUM_CLASSES = sizeof(CLASSES) / sizeof(Class);

PalletWindow::PalletWindow()
    : mFocusedClass(0), mCallbackFunc(NULL), mUserdata(NULL) {

    draw();    
    cv::setMouseCallback(PALLET_WINDOW_NAME, onPalletChanged, this);
}

void PalletWindow::setCallback(OnClassChangedFunc func, void* userdata) {
    mCallbackFunc = func;
    mUserdata = userdata;
}

const cv::Scalar& PalletWindow::getFocusedClassColor() const {
    return CLASSES[mFocusedClass].color;
}

const cv::Scalar& PalletWindow::getFocusedClassLabel() const {
    return CLASSES[mFocusedClass].label;
}

void PalletWindow::draw() {
    cv::Mat img = cv::Mat::zeros(PALLTE_WINDOW_HEIGHT, PALLTE_WINDOW_WIDTH, CV_8UC3);
    
    for (int i = 0; i < NUM_CLASSES; i++) {
        cv::Mat roi = img(CLASSES[i].rect);
        roi.setTo(CLASSES[i].color);
        
        if (mFocusedClass == i) {
            cv::Rect focusRect(
                    cv::Point(FOCUSED_RECT_MERGIN, FOCUSED_RECT_MERGIN),
                    cv::Point(roi.cols-FOCUSED_RECT_MERGIN, roi.rows-FOCUSED_RECT_MERGIN));
            cv::rectangle(roi, focusRect.tl(), focusRect.br(), cv::Scalar(128,128, 255), 3);
        }
        
        cv::putText(roi, std::to_string(i), cv::Point(0, NUMBER_MERGIN_Y),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255));
    }
    
    cv::imshow(PALLET_WINDOW_NAME, img);
}

void PalletWindow::onPalletChanged(int eventType, int x, int y, int flags, void* userdata) {
    PalletWindow* self = (PalletWindow*)userdata;

    cv::Point touchPos(x, y);
    if (eventType == cv::EVENT_LBUTTONDOWN) {
        int tochedClassIndex = -1;
        for (int i = 0; i < NUM_CLASSES; i++) {
            if (CLASSES[i].rect.contains(touchPos)) {
                tochedClassIndex = i;
                break;
            }
        }
        
        if (tochedClassIndex >= 0 && tochedClassIndex < NUM_CLASSES) {
            self->mFocusedClass = tochedClassIndex;
            self->draw();
            if (self->mCallbackFunc) {
                self->mCallbackFunc(self->mUserdata);
            }
        }
    }
}
