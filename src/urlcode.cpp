#include "uricode.h"

namespace URLCode
{

unsigned char ToHex(unsigned char ch)
{
    return ch > 9 ? ch + 55 : ch + 48;
}


unsigned char FromHex(unsigned char ch)
{
    switch(ch)
    {
        case 'A' ... 'Z':
            return ch - 'A' + 10;
        case 'a' ... 'z':
            return ch - 'a' + 10;
        case '0' ... '9':
            return ch - '0';
        default:
            return 0;
    }
}

std::string UriEncode(std::string& str)
{
    std::string strTmp = "";
    for(int i=0;i<str.length();i++)
    {
        if(isalnum((unsigned char)(str[i])) || (str[i] == '-') || (str[i] == '_') || (str[i] == '.') || (str[i] == '~'))
            strTmp += str[i];
        else if(str[i] == ' ')
            strTmp += '+';
        else
        {
                strTmp += '%';
                strTmp += ToHex()
        }
    }
}

}


