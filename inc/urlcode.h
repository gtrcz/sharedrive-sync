#ifndef __URI_CODE_H
#define __URI_CODE_H

#include <string>

namespace URI
{

unsigned char ToHex(unsigned char ch);

unsigned char FromHex(unsigned char ch);

std::string UriEncode(std::string& str);
std::string UriDecode(std::string& str);


}


#endif


