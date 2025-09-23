#ifndef PARSE_JSON_H
#define PARSE_JSON_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ParseJson {
    public:
        ParseJson()=default;
        ~ParseJson()=default;

        ParseJson(std::string jsonPath, std::string detName);
        std::vector<std::string> subRuns();
        std::vector<std::string> decodeFiles(std::string subRun);
        // std::string recoPath(); could be added to json
        // std::string mapPath(); could be added to json
        std::string runName();
        int feu();
        int x1Dream();
        int x2Dream();
        int y1Dream();
        int y2Dream();

    private:
        std::string jsonPath, detName;
        json strJson, detInfo;
};

#endif