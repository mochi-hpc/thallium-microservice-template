/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_RESOURCE_INTERFACE_HPP
#define __ALPHA_RESOURCE_INTERFACE_HPP

#include <alpha/Result.hpp>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include <thallium.hpp>

// TUTORIAL
// ********
//
// The ResourceInterface is an interface class that an implementation
// of a resource must inherit from. The ResourceInterface exposes a number
// of functionalities that the provider will make remotely accessibly.
// These functionalities may not match the ResourceHandle's API one-to-one.
// In the follow example, while the ResourceHandle has 4 versions
// of a computeSum* functionality (to handle timeouts, vectors, bulk handles,
// etc.) this turns into 2 different RPCs that the provider can receive,
// which turn into 1 call to the Resource's computeSum method.
//
// In general, it would be common for the provider to handle things like
// bulk transfers before and after calls to the Resource's API.

namespace alpha {

/**
 * @brief Helper class to register backend types into the backend factory.
 */
template<typename ResourceInterfaceType>
class __AlphaBackendRegistration;

/**
 * @brief Interface for resource backends. To build a new backend,
 * implement a class MyResourceInterface that inherits from ResourceInterface, and put
 * ALPHA_REGISTER_BACKEND(mybackend, MyResourceInterface); in a cpp file
 * that includes your backend class' header file.
 *
 * Your backend class should also have a static function to create a Resource instance:
 *
 * std::unique_ptr<ResourceInterface> Create(const thallium::engine& engine, const json& config)
 */
class ResourceInterface {

    template<typename ResourceInterfaceType>
    friend class __AlphaBackendRegistration;

    std::string m_name;

    public:

    /**
     * @brief Constructor.
     */
    ResourceInterface() = default;

    /**
     * @brief Move-constructor.
     */
    ResourceInterface(ResourceInterface&&) = default;

    /**
     * @brief Copy-constructor.
     */
    ResourceInterface(const ResourceInterface&) = default;

    /**
     * @brief Move-assignment operator.
     */
    ResourceInterface& operator=(ResourceInterface&&) = default;

    /**
     * @brief Copy-assignment operator.
     */
    ResourceInterface& operator=(const ResourceInterface&) = default;

    /**
     * @brief Destructor.
     */
    virtual ~ResourceInterface() = default;

    /**
     * @brief Return the name of backend.
     */
    const std::string& name() const {
        return m_name;
    }

    /**
     * @brief Returns a JSON-formatted configuration string.
     */
    virtual std::string getConfig() const = 0;

    /**
     * @brief Compute the sum of two integers.
     *
     * @param x first integer
     * @param y second integer
     *
     * @return a Result containing the result.
     */
    virtual Result<int32_t> computeSum(int32_t x, int32_t y) = 0;

};

/**
 * @brief The ResourceFactory contains functions to create
 * or open resources.
 */
class ResourceFactory {

    template<typename ResourceInterfaceType>
    friend class __AlphaBackendRegistration;

    using json = nlohmann::json;

    public:

    ResourceFactory() = delete;

    /**
     * @brief Creates a resource and returns a unique_ptr to the created instance.
     *
     * @param backend_name Name of the backend to use.
     * @param engine Thallium engine.
     * @param config Configuration object to pass to the backend's create function.
     *
     * @return a unique_ptr to the created Resource.
     */
    static std::unique_ptr<ResourceInterface> createResource(
            const std::string& backend_name,
            const thallium::engine& engine,
            const json& config);

    private:

    static std::unordered_map<std::string,
                std::function<std::unique_ptr<ResourceInterface>(const thallium::engine&, const json&)>> create_fn;

};

#define ALPHA_REGISTER_BACKEND(__backend_name, __backend_type) \
    static ::alpha::__AlphaBackendRegistration<__backend_type> __alpha ## __backend_name ## _backend( #__backend_name )

template<typename ResourceInterfaceType>
class __AlphaBackendRegistration {

    using json = nlohmann::json;

    public:

    __AlphaBackendRegistration(const std::string& backend_name)
    {
        alpha::ResourceFactory::create_fn[backend_name] = [backend_name](const thallium::engine& engine, const json& config) {
            auto p = ResourceInterfaceType::Create(engine, config);
            p->m_name = backend_name;
            return p;
        };
    }
};

} // namespace alpha

#endif
