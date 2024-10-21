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

/**
 * @brief Helper class to register backend types into the backend factory.
 */
template<typename ResourceInterfaceType>
class __AlphaBackendRegistration;

namespace alpha {

/**
 * @brief Interface for resource backends. To build a new backend,
 * implement a class MyResourceInterface that inherits from ResourceInterface, and put
 * ALPHA_REGISTER_BACKEND(mybackend, MyResourceInterface); in a cpp file
 * that includes your backend class' header file.
 *
 * Your backend class should also have two static functions to
 * respectively create and open a resource:
 *
 * std::unique_ptr<ResourceInterface> create(const json& config)
 * std::unique_ptr<ResourceInterface> attach(const json& config)
 */
class ResourceInterface {

    template<typename ResourceInterfaceType>
    friend class ::__AlphaBackendRegistration;

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

    /**
     * @brief Destroys the underlying resource.
     *
     * @return a Result<bool> instance indicating
     * whether the database was successfully destroyed.
     */
    virtual Result<bool> destroy() = 0;

};

/**
 * @brief The ResourceFactory contains functions to create
 * or open resources.
 */
class ResourceFactory {

    template<typename ResourceInterfaceType>
    friend class ::__AlphaBackendRegistration;

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

    /**
     * @brief Opens an existing resource and returns a unique_ptr to the
     * created backend instance.
     *
     * @param backend_name Name of the backend to use.
     * @param engine Thallium engine.
     * @param config Configuration object to pass to the backend's open function.
     *
     * @return a unique_ptr to the created ResourceInterface.
     */
    static std::unique_ptr<ResourceInterface> openResource(
            const std::string& backend_name,
            const thallium::engine& engine,
            const json& config);

    private:

    static std::unordered_map<std::string,
                std::function<std::unique_ptr<ResourceInterface>(const thallium::engine&, const json&)>> create_fn;

    static std::unordered_map<std::string,
                std::function<std::unique_ptr<ResourceInterface>(const thallium::engine&, const json&)>> open_fn;
};

} // namespace alpha


#define ALPHA_REGISTER_BACKEND(__backend_name, __backend_type) \
    static __AlphaBackendRegistration<__backend_type> __alpha ## __backend_name ## _backend( #__backend_name )

template<typename ResourceInterfaceType>
class __AlphaBackendRegistration {

    using json = nlohmann::json;

    public:

    __AlphaBackendRegistration(const std::string& backend_name)
    {
        alpha::ResourceFactory::create_fn[backend_name] = [backend_name](const thallium::engine& engine, const json& config) {
            auto p = ResourceInterfaceType::create(engine, config);
            p->m_name = backend_name;
            return p;
        };
        alpha::ResourceFactory::open_fn[backend_name] = [backend_name](const thallium::engine& engine, const json& config) {
            auto p = ResourceInterfaceType::open(engine, config);
            p->m_name = backend_name;
            return p;
        };
    }
};

#endif
