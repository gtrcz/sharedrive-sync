#ifndef __ALIST_H
#define __ALIST_H

#include <string>

class Alist
{
public:
    Alist();
    ~Alist();
public:
    std::string GetFileLists();
public:
    std::string GetUserToken();
    std::string ListDir();
private:
    //std::string CurlGet();
    //std::string CurlPost();

    std::string EncodeURI(std::string& str);
    std::string DecodeURI(std::string& str);
private:
    std::string userToken = "";
};



#endif
