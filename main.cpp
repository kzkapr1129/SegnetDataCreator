//
//  main.cpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#include "CanvasWindow.h"
#include "utility.h"
#include "PalletWindow.h"
#include <opencv2/opencv.hpp>
#include <sys/stat.h>

static const int ACCESS_FLAG = S_IRGRP | S_IROTH | S_IRUSR | S_IRWXO | S_IRWXU | S_IWOTH;

struct CommonData {
    std::string arg_inputDir;
    std::string arg_outputImgDir;
    std::string arg_outputLabelDir;
    std::string arg_outputText;
    int arg_outputImgSize;
    
    std::vector<std::string> filelist;
    std::string currentFilename;
    size_t seek;
    
    CanvasWindow* canvas;
    PalletWindow* pallet;
    
    CommonData(int argc, const char* argv[], CanvasWindow* c, PalletWindow* p)
            : seek(0), canvas(c), pallet(p) {
        arg_inputDir = appendSlashIfNessesory(argv[1]);
        arg_outputImgDir = appendSlashIfNessesory(argv[2]);
        arg_outputLabelDir = appendSlashIfNessesory(argv[3]);
        arg_outputText = argv[4];
        arg_outputImgSize = atoi(argv[5]);

        getFileList(arg_inputDir, filelist, argv[6]);
    }
    
    bool nextSqureImage(cv::Mat& output) {
        if (seek >= filelist.size()) {
            return false;
        }
        
        // 画像の読み込み
        currentFilename = filelist[seek++];
        cv::Mat img = cv::imread(currentFilename);
        
        // 正四角形の画像にする
        int squreSize = std::min(img.rows, img.cols);
        cv::Rect roiRect(0, 0, squreSize, squreSize);
        cv::Mat roi = img(roiRect);
        
        // 画像のリサイズする
        cv::resize(roi, output, cv::Size(arg_outputImgSize, arg_outputImgSize));
        
        return true;
    }
};

static void genFilename(const CommonData& data, std::string& output) {
    // 現在時刻を取得
    time_t now;
    now = time(NULL);
    
    // 1秒間の間に複数枚の画像が保存されることを考慮し、ファイル名にインデックス番号を付与する名前を生成する
    char filename[255];
    sprintf(filename, "%ld_%03ld", now, data.seek % 100);
    
    // ファイル名を返却する
    output = std::string(filename);
}

static void onSaveCallback(void* userdata, const cv::Mat& src, const cv::Mat& label) {
    CommonData* data = (CommonData*)userdata;
    if (data->currentFilename.length() > 0) {
        remove(data->currentFilename.c_str());
    }
    
    mkdir(data->arg_outputImgDir.c_str(), ACCESS_FLAG);
    mkdir(data->arg_outputLabelDir.c_str(), ACCESS_FLAG);
    
    std::string baseFilename;
    genFilename(*data, baseFilename);
    
    std::string imgFile = baseFilename + ".png";
    std::string labelFile = baseFilename + "_label.png";
    
    std::string imgPath   = data->arg_outputImgDir   + imgFile;
    std::string labelPath = data->arg_outputLabelDir + labelFile;
    
    try {
        cv::imwrite(imgPath.c_str(), src);
        cv::imwrite(labelPath.c_str(), label);
    } catch (cv::Exception e) {
        printf("failed to save image: %s\n", e.what());
    }
    
    FILE* fp = fopen(data->arg_outputText.c_str(), "a");
    if (fp) {
        fprintf(fp, "%s,%s\n", imgFile.c_str(), labelFile.c_str());
        fclose(fp);
    }
}

static void onClassChanged(void* userdata) {
    CommonData* data = (CommonData*)userdata;
    const cv::Scalar& cc = data->pallet->getFocusedClassColor();
    const cv::Scalar& lc = data->pallet->getFocusedClassLabel();
    data->canvas->setColor(cc, lc);
}

int main(int argc, const char * argv[]) {
    if (argc != 7) {
        printf("Usage: %s <input_dir> <output_img_dir> <output_label_dir> <output.txt> <output_img_size> <extention>", argv[0]);
        return 0;
    }
    
    // パレットウインドウの表示
    PalletWindow pallet;
    
    // キャンパスウインドウの表示
    CanvasWindow canvas;
    
    // データの初期化
    CommonData data(argc, argv, &canvas, &pallet);
    
    // コールバックの登録
    pallet.setCallback(onClassChanged, &data);
    canvas.setCallback(onSaveCallback, &data);

    // キャンパスに色を設定する
    const cv::Scalar& classColor = pallet.getFocusedClassColor();
    const cv::Scalar& labelColor = pallet.getFocusedClassLabel();
    canvas.setColor(classColor, labelColor);
    
    while (true) {
        
        if (canvas.isNeededImage()) {
            cv::Mat img;
            if (!data.nextSqureImage(img)) {
                // 読み込む画像がないので終了
                break;
            }
            
            canvas.setImage(img);
        }
        
        int key = cv::waitKey(1);
        if (key == 27) {
            // ESCキーが押下されたので終了
            break;
        } else if (key == 'c') {
            canvas.clear();
        }
    }
    
    
    return 0;
}
