//
//  utility.hpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

bool splitFilename(const std::string& filename, std::string& name, std::string& ext);
bool endWith(const std::string& str, const std::string& suffix);
std::string upperString(const std::string& str);
bool getFileList(const std::string& dir, std::vector<std::string>& filelist, const std::string& ext);
std::string appendSlashIfNessesory(const std::string& str);

