/*
 * (C) 2024 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_RESULT_HPP
#define __ALPHA_RESULT_HPP

#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>
#include <alpha/Exception.hpp>
#include <string>

namespace alpha {

// TUTORIAL
// ********
//
// The Result class is used as response type to all the RPCs in this project.
// It acts like a variant with a main type T and an alpha::Exception for
// error handling. RPCs will generally either pass their return value as T
// or propagate an exception to be thrown by the client.

/**
 * @brief The Result object is a generic object
 * used to hold and send back the result of an RPC.
 * It contains three fields:
 * - success must be set to true if the request succeeded, false otherwise
 * - error must be set to an error string if an error occured
 * - value must be set to the result of the request if it succeeded
 *
 * This class is specialized for two types: bool and std::string.
 * If bool is used, both the value and the success fields will be
 * managed by the same underlying variable. If std::string is used,
 * both the value and the error fields will be managed by the same
 * underlying variable.
 *
 * @tparam T Type of the result.
 */
template<typename T>
class Result {

    template<typename U>
    friend class Result;

    public:

    Result() = default;

    template<typename U>
    Result(Result<U>&& other)
    : m_success{other.m_success}
    , m_error{std::move(other.m_error)}
    , m_value{std::move(other.m_value)} {}

    template<typename U>
    Result(const Result<U>& other)
    : m_success{other.m_success}
    , m_error{other.m_error}
    , m_value{other.m_value} {}

    template<typename U>
    Result& operator=(Result<U>&& other) {
        if(this == reinterpret_cast<decltype(this)>(&other)) return *this;
        m_success = other.m_success;
        m_error   = std::move(other.m_error);
        m_value   = std::move(other.m_value);
        return *this;
    }

    template<typename U>
    Result& operator=(const Result<U>& other) {
        if(this == reinterpret_cast<decltype(this)>(&other)) return *this;
        m_success = other.m_success;
        m_error   = other.m_error;
        m_value   = other.m_value;
        return *this;
    }

    /**
     * @brief Whether the request succeeded.
     */
    bool& success() {
        return m_success;
    }

    /**
     * @brief Whether the request succeeded.
     */
    const bool& success() const {
        return m_success;
    }

    /**
     * @brief Error string if the request failed.
     */
    std::string& error() {
        return m_error;
    }

    /**
     * @brief Error string if the request failed.
     */
    const std::string& error() const {
        return m_error;
    }

    /**
     * @brief Value if the request succeeded.
     */
    T& value() {
        return m_value;
    }

    /**
     * @brief Value if the request succeeded.
     */
    const T& value() const {
        return m_value;
    }

    /**
     * @brief Value if the request succeeded,
     * throws otherwise.
     */
    const T& valueOrThrow() const & {
        check();
        return m_value;
    }

    /**
     * @brief Value if the request succeeded,
     * throws otherwise.
     */
    T&& valueOrThrow() && {
        check();
        return std::move(m_value);
    }

    /**
     * @brief Execute a function on the value
     * if the value is present, otherwise throws
     * an Exception.
     */
    template<typename F>
    decltype(auto) andThen(F&& f) const & {
        return std::forward<F>(f)(valueOrThrow());
    }

    /**
     * @brief Execute a function on the value
     * if the value is present, otherwise throws
     * an Exception.
     */
    template<typename F>
    decltype(auto) andThen(F&& f) && {
        return std::forward<F>(f)(valueOrThrow());
    }

    /**
     * @brief Throw an Exception if the Result
     * contains an error.
     */
    void check() const {
        if(!m_success)
            throw Exception(m_error);
    }

    /**
     * @brief Serialization function for Thallium.
     *
     * @tparam Archive Archive type.
     * @param a Archive instance.
     */
    template<typename Archive>
    void serialize(Archive& a) {
        a & m_success;
        if(m_success) {
            a & m_value;
        } else {
            a & m_error;
        }
    }

    private:

    bool        m_success = true;
    std::string m_error   = "";
    T           m_value;
};

template<>
class Result<void> {

    template<typename U>
    friend class Result;

    public:

    Result() = default;

    template<typename U>
    Result(Result<U>&& other)
    : m_success{other.m_success}
    , m_error{std::move(other.m_error)} {}

    template<typename U>
    Result(const Result<U>& other)
    : m_success{other.m_success}
    , m_error{other.m_error} {}

    template<typename U>
    Result& operator=(Result<U>&& other) {
        if(this == reinterpret_cast<decltype(this)>(&other)) return *this;
        m_success = other.m_success;
        m_error   = std::move(other.m_error);
        return *this;
    }

    template<typename U>
    Result& operator=(const Result<U>& other) {
        if(this == reinterpret_cast<decltype(this)>(&other)) return *this;
        m_success = other.m_success;
        m_error   = other.m_error;
        return *this;
    }

    /**
     * @brief Whether the request succeeded.
     */
    bool& success() {
        return m_success;
    }

    /**
     * @brief Whether the request succeeded.
     */
    const bool& success() const {
        return m_success;
    }

    /**
     * @brief Error string if the request failed.
     */
    std::string& error() {
        return m_error;
    }

    /**
     * @brief Error string if the request failed.
     */
    const std::string& error() const {
        return m_error;
    }

    /**
     * @brief Execute a function on the value
     * if the value is present, otherwise throws
     * an Exception.
     */
    template<typename F>
    decltype(auto) andThen(F&& f) const & {
        check();
        return std::forward<F>(f)();
    }

    /**
     * @brief Execute a function on the value
     * if the value is present, otherwise throws
     * an Exception.
     */
    template<typename F>
    decltype(auto) andThen(F&& f) && {
        check();
        return std::forward<F>(f)();
    }

    /**
     * @brief Throw an Exception if the Result
     * contains an error.
     */
    void check() const {
        if(!m_success)
            throw Exception(m_error);
    }

    /**
     * @brief Serialization function for Thallium.
     *
     * @tparam Archive Archive type.
     * @param a Archive instance.
     */
    template<typename Archive>
    void serialize(Archive& a) {
        a & m_success;
        if(!m_success) {
            a & m_error;
        }
    }

    private:

    bool        m_success = true;
    std::string m_error   = "";
};

}

#endif
