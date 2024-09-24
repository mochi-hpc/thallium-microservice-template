/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_RESOURCE_HANDLE_HPP
#define __ALPHA_RESOURCE_HANDLE_HPP

#include <thallium.hpp>
#include <memory>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <alpha/Client.hpp>
#include <alpha/Exception.hpp>
#include <alpha/Future.hpp>

namespace alpha {

class Client;
class ResourceHandleImpl;

/**
 * @brief A ResourceHandle object is a handle for a remote resource
 * on a server. It enables invoking the resource's functionalities.
 */
class ResourceHandle {

    friend class Client;

    public:

    /**
     * @brief Constructor. The resulting ResourceHandle handle will be invalid.
     */
    ResourceHandle();

    /**
     * @brief Copy-constructor.
     */
    ResourceHandle(const ResourceHandle&);

    /**
     * @brief Move-constructor.
     */
    ResourceHandle(ResourceHandle&&);

    /**
     * @brief Copy-assignment operator.
     */
    ResourceHandle& operator=(const ResourceHandle&);

    /**
     * @brief Move-assignment operator.
     */
    ResourceHandle& operator=(ResourceHandle&&);

    /**
     * @brief Destructor.
     */
    ~ResourceHandle();

    /**
     * @brief Returns the client this resource has been opened with.
     */
    Client client() const;


    /**
     * @brief Checks if the ResourceHandle instance is valid.
     */
    operator bool() const;

    /**
     * @brief Requests the target resource to compute the sum of two numbers.
     * If result is null, it will be ignored. If req is not null, this call
     * will be non-blocking and the caller is responsible for waiting on
     * the request.
     *
     * @param[in] x first integer
     * @param[in] y second integer
     *
     * @return a Future<int32_t> that can be awaited to get the result.
     */
    Future<int32_t> computeSum(int32_t x, int32_t y) const;

    private:

    /**
     * @brief Constructor is private. Use a Client object
     * to create a ResourceHandle instance.
     *
     * @param impl Pointer to implementation.
     */
    ResourceHandle(const std::shared_ptr<ResourceHandleImpl>& impl);

    std::shared_ptr<ResourceHandleImpl> self;
};

}

#endif
