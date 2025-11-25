/*
 * (C) 2024 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_FUTURE_HPP
#define __ALPHA_FUTURE_HPP

#include <alpha/Exception.hpp>
#include <alpha/Result.hpp>
#include <thallium.hpp>
#include <memory>
#include <functional>

namespace alpha {

// TUTORIAL
// ********
//
// The Future object is what is returned by all the RPC operations on
// the ResourceHandle. While thallium/margo provide blocking RPCs, we
// recommend using the non-blocking version (rpc.async(...) in thallium)
// to give callers an opportunity to do useful work while the RPC is
// executing. The Future object encapsulates the thallium::async_response
// instance returned by the rpc.async(...) call.
//
// The Future class is templated with two types: T and Wrapper.
// T is the type ultimately returned by the wait() function, but not
// necessarily the type returned by the RPC itself. The RPC is expected
// to return a Result<Wrapper>, with Wrapper being a type that can be
// implicitly converted into a T instance.

/**
 * @brief Future objects are used to keep track of
 * on-going asynchronous operations.
 */
template<typename T, typename Wrapper = T>
class Future {

    public:

    /**
     * @brief Copy constructor.
     */
    Future() = default;

    /**
     * @brief Copy constructor.
     */
    Future(const Future& other) = default;

    /**
     * @brief Move constructor.
     */
    Future(Future&& other) = default;

    /**
     * @brief Copy-assignment operator.
     */
    Future& operator=(const Future& other) = default;

    /**
     * @brief Move-assignment operator.
     */
    Future& operator=(Future&& other) = default;

    /**
     * @brief Destructor.
     */
    ~Future() = default;

    /**
     * @brief Wait for the request to complete.
     */
    T wait() {
        try {
            Result<Wrapper> result = m_resp.wait();
            if constexpr (!std::is_same_v<T, void>) {
                return std::move(result).valueOrThrow();
            } else {
                std::move(result).check();
            }
        } catch(const thallium::timeout&) {
            throw Exception{"Operation timed out"};
        }
    }

    /**
     * @brief Test if the request has completed, without blocking.
     */
    bool completed() const {
        try {
            return m_resp.received();
        } catch(const thallium::timeout&) {
            throw Exception{"Operation timed out"};
        }
    }

    /**
     * @brief Constructor.
     */
    Future(thallium::async_response resp)
    : m_resp(std::move(resp)) {}

    private:

    thallium::async_response m_resp;
};

}

#endif
