#pragma once

#include <exception>

namespace xvision {

enum ErrCode {
    
};

#define XVISION_FMT_EXCEPTION(code, ...)

class Exception : public std::exception {
};

} // namespace xvision
