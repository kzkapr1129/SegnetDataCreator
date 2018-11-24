//
//  utility.cpp
//  SegnetTestDataCreator
//
//  Created by 中山一輝 on 2018/11/24.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#include "utility.h"
#include <dirent.h>

bool splitFilename(const std::string& filename, std::string& name, std::string& ext) {
    auto ei = filename.find_last_of('.');
    if (ei == std::string::npos) {
        return false;
    }
    
    name = filename.substr(0, ei);
    ext = filename.substr(ei);
    
    return true;
}

bool endWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    std::string upper = upperString(str);
    return std::equal(std::rbegin(suffix), std::rend(suffix), std::rbegin(upper));
}

std::string upperString(const std::string& str) {
    std::string upper_string;
    upper_string.resize(str.size());
    std::transform(str.cbegin(), str.cend(), upper_string.begin(), toupper);
    return upper_string;
}

bool getFileList(const std::string& dir, std::vector<std::string>& filelist, const std::string& ext) {
    std::string uext = upperString(ext);
    
    DIR* dp = opendir(dir.c_str());
    if (dp != NULL) {
        struct dirent* dent;
        do {
            dent = readdir(dp);
            if (dent != NULL && dent->d_type == DT_REG) {
                if (dent->d_name[0] != '.') {
                    std::string filename = dent->d_name;
                    if (endWith(filename, uext)) {
                        filelist.push_back(dir + dent->d_name);
                    }
                }
            }
            
        } while (dent != NULL);
        
        closedir(dp);
    }
    
    return filelist.size() > 0;
}

std::string appendSlashIfNessesory(const std::string& str) {
    std::string::size_type len = str.length();
    if (len <= 0) {
        return str;
    }

    if (str[len-1] != '/') {
        return str + "/";
    } else {
        return str;
    }
}
