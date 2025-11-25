/*
 * (C) 2024 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "alpha/Client.hpp"
#include "alpha/Provider.hpp"

#include <bedrock/AbstractComponent.hpp>

namespace tl = thallium;

class AlphaComponent : public bedrock::AbstractComponent {

    std::unique_ptr<alpha::Provider> m_provider;

    public:

    AlphaComponent(const tl::engine& engine,
                    uint16_t  provider_id,
                    const std::string& config,
                    const tl::pool& pool)
    : m_provider{std::make_unique<alpha::Provider>(engine, provider_id, config, pool)}
    {}

    void* getHandle() override {
        return static_cast<void*>(m_provider.get());
    }

    std::string getConfig() override {
        return m_provider->getConfig();
    }

    static std::shared_ptr<bedrock::AbstractComponent>
        Register(const bedrock::ComponentArgs& args) {
            tl::pool pool;
            auto it = args.dependencies.find("pool");
            if(it != args.dependencies.end() && !it->second.empty()) {
                pool = it->second[0]->getHandle<tl::pool>();
            }
            return std::make_shared<AlphaComponent>(
                args.engine, args.provider_id, args.config, pool);
        }

    static std::vector<bedrock::Dependency>
        GetDependencies(const bedrock::ComponentArgs& args) {
            (void)args;
            std::vector<bedrock::Dependency> dependencies{
                bedrock::Dependency{
                    /* name */ "pool",
                    /* type */ "pool",
                    /* is_required */ false,
                    /* is_array */ false,
                    /* is_updatable */ false
                }
            };
            return dependencies;
        }
};

BEDROCK_REGISTER_COMPONENT_TYPE(alpha, AlphaComponent)
