/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_EXCEPTION_HPP
#define __ALPHA_EXCEPTION_HPP

#include <exception>
#include <string>

namespace alpha {

// TUTORIAL
// ********
//
// A single Exception class is provided for all the exceptions in the project.
// This class has a serialization function so it can be serialized as an RPC
// response or argument.

class Exception : public std::exception {

    std::string m_error;

    public:

    template<typename ... Args>
    Exception(Args&&... args)
    : m_error(std::forward<Args>(args)...) {}

    virtual const char* what() const noexcept override {
        return m_error.c_str();
    }

    template<typename Archive>
    void serialize(Archive& ar) {
        ar(m_error);
    }
};

}

#endif
