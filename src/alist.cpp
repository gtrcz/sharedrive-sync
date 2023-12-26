#include "alist.h"
#include "json.h"
#include "curl.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>


Alist::Alist()
{
    this->userToken = GetUserToken();
}

Alist::~Alist()
{

}

std::string Alist::EncodeURI(std::string& str)
{
    auto encodeValue = curl_easy_escape(nullptr, str.c_str(), str.length());
    std::string res(encodeValue);
    cur_free(encodeValue);
    return res;
}

std::string Alist::DecodeURI(std::string& str)
{
    int length;
    auto decodeValue = culr_easy_unescape(nullptr, str.c_str(),str.length(), &length);
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
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.216.132:5244/api/auth/login");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        const char *data = "{\n    \"username\": \"user\",\n    \"password\": \"user_1234\"\n}";
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



std::string Alist::GetFileLists()
{
    return ListDir();

}

std::string Alist::ListDir()
{

    if(this->userToken.empty())
    {
        std::cout<<"empty userToken!\n";
        return "";
    }

    std::string responseStr = "";
    //1.list dir
    {
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.216.132:5244/api/fs/list");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
            struct curl_slist *headers = NULL;
            std::string authToken = "Authorization: " + this->userToken;
            headers = curl_slist_append(headers, authToken.c_str());
            headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            const char *data = "{\n    \"path\": \"/books\",\n    \"refresh\": false\n}";
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
    }
    
    if(responseStr.empty())
    {
        std::cout<<"invalid response data\n";
        return "";
    }
    //2.traverse dir
    {
        std::stringstream fileListStream;
        std::fstream outputFile;
        outputFile.open("books.json", std::ios::out);


        std::stringstream responseStream(responseStr);
        Json::Value root;
        responseStream >> root;
        
        int totalFiles = root["data"]["total"].asUInt();
        fileListStream << "[\n";
        for(int i=0;i<totalFiles;i++)
        {
            fileListStream << "{\n";

            fileListStream << "\"id\": ";
            fileListStream << i;
            fileListStream << ",\n";

            fileListStream << "\"date\": ";
            fileListStream << "\"";
            fileListStream << root["data"]["content"][i]["created"].asString();
            fileListStream << "\",\n";

            fileListStream << "\"name\": ";
            fileListStream << "\"";
            fileListStream << root["data"]["content"][i]["name"].asString();
            fileListStream << "\",\n";

            fileListStream << "\"url\": ";
            fileListStream << "\"";
            fileListStream << "http://192.168.216.132:5244/d/alidrive/books/";
            fileListStream << EncodeURI(root["data"]["content"][i]["name"].asString());
            fileListStream << "\",\n";

            fileListStream << "\"size\": ";
            fileListStream << "\"";
            fileListStream << (root["data"]["content"][i]["size"].asUInt()) / 1024.0;
            fileListStream << "\"\n";
            
            fileListStream << "}\n";

            if((i+1) < totalFiles)
                fileListStream << ",";

        }

        fileListStream << "]\n";
        outputFile << fileListStream.str();

        outputFile.close();

        return fileListStream.str();
    }


}



