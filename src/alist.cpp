#include "alist.h"
#include "json.h"
#include "curl.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>


Alist::Alist(std::string& configStr)
{
    ParseConfig(configStr);

    this->requestUrl = "http://" + configMap["server"] + ":" + configMap["port"] + "/api/fs/list";
    this->baseDownloadUrl = "http://" + configMap["server"] + ":" + configMap["port"] + "/d/";
    this->authUrl = "http://" + configMap["server"] + ":" + configMap["port"] + "/api/auth/login";
    
    this->userToken = GetUserToken();

}

Alist::~Alist()
{

}

std::string Alist::EncodeURI(std::string str)
{
    auto encodeValue = curl_easy_escape(nullptr, str.c_str(), str.length());
    std::string res(encodeValue);
    curl_free(encodeValue);
    return res;
}

std::string Alist::DecodeURI(std::string str)
{
    int length;
    auto decodeValue = curl_easy_unescape(nullptr, str.c_str(),str.length(), &length);
    std::string res(decodeValue, length);
    curl_free(decodeValue);
    return res;

}


size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    ((std::string*)userdata)->append(ptr, nmemb);
    return nmemb;
}


std::string Alist::GetUserToken()
{
    std::string responseStr = "";
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, this->authUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        const char *data = "{\n    \"username\": \"user\",\n    \"password\": \"user_alist_2023\"\n}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            std::cout<<"curl_easy_perform() error: "<<curl_easy_strerror(res)<<"\n";
            responseStr = "";
        }
        else
        {
            long response_code;
            curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
            
            std::cout<<"response data: "<<responseStr<<"\n";

        }

        curl_slist_free_all(headers);
    }
    
    curl_easy_cleanup(curl);

    {
        std::stringstream responseStream(responseStr);
        Json::Value root;
        responseStream >> root;
        responseStr = root["data"]["token"].asString();
        std::cout<<responseStr<<"\n";
    }
    return responseStr;

}


int32_t Alist::ParseConfig(std::string& configStr)
{
    std::stringstream configStream(configStr);
    Json::Value root;
    configStream >> root;

    configMap["server"] = root["server"].asString();
    configMap["port"] = root["port"].asString();
    for(int i=0;i<root["drives"].size();i++)
        drives.push_back(root["drives"][i].asString());

    return 0;
}


int32_t Alist::save(std::string& str)
{
    std::fstream fs;
    fs.open("books.json", std::ios::out);
    if(!fs.is_open())
    {
        std::cout<<"open file error!\n";
        return -1;
    }

    fs << str;

    fs.close();

    return 0;

}

std::string Alist::ListDir(std::string rootPath)
{
    if(this->userToken.empty())
    {
        std::cout<<"invalid userToken\n";
        return "";
    }

    std::string responseStr = "";

    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, this->requestUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

        struct curl_slist* headers = NULL;
        std::string authToken = "Authorization: " + this->userToken;
        headers = curl_slist_append(headers, authToken.c_str());
        headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        char buf[1024] = {0};
        sprintf(buf, "{\n    \"path\": \"%s\",\n    \"refresh\": false\n}", rootPath.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            std::cout<<"curl_easy_perform() error: "<<curl_easy_strerror(res)<<"\n";
            responseStr = "";
        }
        else
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout<<"response data: "<<responseStr<<":\t"<<(rootPath)<<"\n";
        }

        curl_slist_free_all(headers);

    }

    curl_easy_cleanup(curl);
    
    return responseStr;
}

int32_t Alist::Search()
{
    std::string fileResult = "[\n";

    for(int i=0;i<drives.size();i++)
    {
        fileResult += Search(drives.at(i));
    }

    //remove the last ','
    fileResult.pop_back();

    fileResult += "\n]\n";
    
    //save
    save(fileResult);

    return 0;
}

std::string Alist::Search(std::string rootPath)
{

    std::string fileResult = "";
    //init
    std::list<std::string> dirList;
    dirList.push_back(rootPath);

    //traverse
    for(auto it = dirList.begin();it!= dirList.end();it++)
    {
        //get dir
        std::string contents = ListDir(*it);
        //parse
        std::list<std::string> subDirs;
        fileResult += Parse(*it, contents, subDirs);
        //update dirList;
        dirList.insert(dirList.end(), subDirs.begin(), subDirs.end());

    }

    return fileResult;

}

std::string Alist::Parse(std::string rootPath, std::string& contents, std::list<std::string>& subDirs)
{
    subDirs.clear();

    if(contents.empty())
    {
        std::cout<<"invalid contents\n";
        return "";
    }

    std::stringstream fileListStream;

    std::stringstream contentStream(contents);
    Json::Value root;
    contentStream >> root;

    int totalFiles = root["data"]["total"].asUInt();


    for(int i=0;i<totalFiles;i++)
    {
        //check dir
        if(root["data"]["content"][i]["is_dir"].asBool())//is dir
        {
            subDirs.push_back(rootPath + "/" + root["data"]["content"][i]["name"].asString());
            continue;
        }

        fileListStream << "\n\t{\n";

        //id
        fileListStream << "\t\t\"id\": ";
        fileListStream << (this->fileCounter ++);
        fileListStream << ",\n";

        //date
        fileListStream << "\t\t\"date\": ";
        fileListStream << "\"";
        fileListStream << root["data"]["content"][i]["created"].asString();
        fileListStream << "\",\n";

        //name
        fileListStream << "\t\t\"name\": ";
        fileListStream << "\"";
        fileListStream << root["data"]["content"][i]["name"].asString();
        fileListStream << "\",\n";

        //url
        fileListStream << "\t\t\"url\": ";
        fileListStream << "\"";
        fileListStream << this->baseDownloadUrl << rootPath << "/";
        fileListStream << EncodeURI(root["data"]["content"][i]["name"].asString());
        fileListStream << "\",\n";

        //size
        fileListStream << "\t\t\"size\": ";
        fileListStream << "\"";

        char buf[32] = {0};
        uint32_t fileSize = root["data"]["content"][i]["size"].asUInt();
        if(fileSize < 1024)// < 1K
            sprintf(buf, "%d B", fileSize);
        else if(fileSize < 1024 * 1024)// < 1M
            sprintf(buf, "%.2f KB", fileSize / 1024.0);
        else if(fileSize < 1024 * 1024 * 1024) // < 1G
            sprintf(buf, "%.2f MB", fileSize / 1024.0 / 1024.0);
        else
            sprintf(buf, "%.2f GB", fileSize / 1024.0 / 1024.0 / 1024.0);

        std::string res(buf);
        fileListStream << res;
        fileListStream << "\"\n";

        //end
        fileListStream << "\t},";
        
    }

    return fileListStream.str();

}