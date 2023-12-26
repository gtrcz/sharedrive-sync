#include "alist.h"
#include <iostream>
#include <fstream>


#include "json.h"

//load config
std::map<std::string,std::string> LoadConfig(std::string file)
{
    std::map<std::string, std::string> res;
    //1.1 open file
    std::ifstream fs(file.c_str(), std::ios::binary);

    if(!fs.is_open())
    {
        std::cout<<"open config file error!\n";
        return res;
    }

    Json::Value root;
    fs >> root;

    res["alist"] = root["alist"].toStyledString();

    return res;
}

int main(int argc, char** argv)
{

    std::string configFile = "./config/config.json";
    if(argc >= 2)
        configFile = std::string(argv[1]);

    //1.load config
    std::map<std::string, std::string> config = LoadConfig(configFile);

    //search
    Alist alist(config["alist"]);
    alist.Search();

    //return
    return -1;
}