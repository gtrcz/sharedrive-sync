#ifndef __ALIST_H
#define __ALIST_H

#include <string>
#include <map>
#include <list>
#include <vector>

class Alist
{
public:
    Alist(std::string& configStr);
    ~Alist();
public:
    //遍历所有drive
    int32_t Search();
private:
    //获取token
    std::string GetUserToken();
    //存储查询结果
    int32_t save(std::string& str);
    //查询指定目录
    std::string ListDir(std::string rootPath);
    //分析目录查询结果
    std::string Parse(std::string rootPath, std::string& contents, std::list<std::string>& subDirs);
    //遍历指定drive
    std::string Search(std::string rootPath);

private:
    int32_t ParseConfig(std::string& configStr);

private:

    std::string EncodeURI(std::string str);
    std::string DecodeURI(std::string str);
private:
    std::string userToken = "";

    std::string requestUrl = "";
    std::string baseDownloadUrl = "";
    std::string authUrl = "";

    int32_t fileCounter = 0;
    
    std::map<std::string, std::string> configMap;
    std::vector<std::string> drives;
};



#endif
