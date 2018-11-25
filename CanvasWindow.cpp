//
//  CanvasWindow.cpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#include "CanvasWindow.h"

static const char* CANVAS_WINDOW_NAME = "class canvas";

CanvasWindow::CanvasWindow()
    : mOnSaveFunc(NULL), mUserdata(NULL), mClassColor(0, 0, 0), mLabelColor(0) {
        
    cv::namedWindow(CANVAS_WINDOW_NAME);
    cv::setMouseCallback(CANVAS_WINDOW_NAME, onMouseEvent, this);
}

void CanvasWindow::setCallback(OnSaveFunc saveFunc, void* userdata) {
    mOnSaveFunc = saveFunc;
    mUserdata = userdata;
}

void CanvasWindow::setImage(const cv::Mat& img) {
    mSrcImg = img.clone();
    clear();
    
    cv::imshow(CANVAS_WINDOW_NAME, mWorkImg);
}

void CanvasWindow::setColor(const cv::Scalar& cc, const cv::Scalar& lc) {
    mClassColor = cc;
    mLabelColor = lc;
    
    clearVertexes();
}

void CanvasWindow::clear() {
    mCommitedImage = mSrcImg.clone();
    mWorkImg = mSrcImg.clone();
    mLabelImg = cv::Mat::zeros(mSrcImg.rows, mSrcImg.cols, CV_8UC1);
    mLabelImg.setTo(cv::Scalar(0));
    mVertexes.clear();
    
    // 再描画
    cv::imshow(CANVAS_WINDOW_NAME, mWorkImg);
    
    // ラベルウインドウの更新
    updateLabelWindow();
}

void CanvasWindow::clearVertexes() {
    mWorkImg = mCommitedImage.clone();
    mVertexes.clear();
    
    // 再描画
    if (mWorkImg.data) cv::imshow(CANVAS_WINDOW_NAME, mWorkImg);
}

bool CanvasWindow::isNeededImage() {
    return mSrcImg.data == NULL;
}

void CanvasWindow::updateLabelWindow() {
    // ラベル画像のデバックウインドウの表示
    cv::Mat label = mLabelImg.clone();
    cv::resize(label, label, cv::Size(), 0.5, 0.5); // 1/2にリサイズする
    cv::equalizeHist(label, label);
    cv::imshow("label debug", label);
}

void CanvasWindow::release() {
    mSrcImg.release();
    mCommitedImage.release();
    mWorkImg.release();
    mLabelImg.release();
    mVertexes.clear();
}

void CanvasWindow::onMouseEvent(int eventType, int x, int y, int flags, void* userdata) {
    CanvasWindow* self = (CanvasWindow*)userdata;
    if (self->isNeededImage()) {
        // 画像を持っていないときは何もせずに終了する
        return;
    }
    
    if (eventType == cv::EVENT_LBUTTONDOWN) {
        // 頂点の打点を行う
        cv::Point vertex(x, y);
        cv::circle(self->mWorkImg, vertex, 2, self->mClassColor, -1);
        self->mVertexes.push_back(vertex);
        
        // 再描画
        cv::imshow(CANVAS_WINDOW_NAME, self->mWorkImg);
        
    } else if (eventType == cv::EVENT_RBUTTONDOWN) {
        // 頂点の中を塗りつぶす (画像のコミット)
        cv::fillConvexPoly(self->mCommitedImage, self->mVertexes, self->mClassColor);
        cv::fillConvexPoly(self->mLabelImg, self->mVertexes, self->mLabelColor);
        
        // 頂点をクリアする
        self->mVertexes.clear();
        
        // 作業用イメージの更新
        self->mWorkImg = self->mCommitedImage.clone();
        
        // 再描画
        cv::imshow(CANVAS_WINDOW_NAME, self->mWorkImg);
        
        // ラベルの表示
        self->updateLabelWindow();
        
    } else if (eventType == cv::EVENT_MBUTTONUP) {
        if (self->mVertexes.size() <= 0) {
            // ラベル画像を保存する
            if (self->mOnSaveFunc) {
                self->mOnSaveFunc(self->mUserdata, self->mSrcImg, self->mLabelImg);
            }
            self->release();
        }
    }
}

