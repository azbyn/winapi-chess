#include "Utils.h"

#include <sstream>
#include <iostream>

namespace core {

std::string WinapiError::makeMessage(std::string_view str, DWORD& code,
                                     const source_location& location) {
    std::stringstream ss;
    ss << location.file_name() << "@"
       << location.line() << ":"
       << location.function_name() << ": "
       << str;
    code = ::GetLastError();
    
    char* msg = nullptr;
    DWORD len = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr,
                                 code,
                                 0, //default language
                                 (LPSTR)&msg,
                                 0,
                                 nullptr);
    ss << "(code " << code << "): ";
    if (msg == nullptr) {
        ss << "No Winapi Error.";
    } else {
        ss.write(msg, len);
        ::LocalFree(msg);
    }
    return ss.str();
}

} // namespace core
