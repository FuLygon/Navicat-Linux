#pragma once
#include "../exception.hpp"

namespace nkg::exceptions {

    class key_exception : public ::nkg::exception {
        using ::nkg::exception::exception;
    };

}
