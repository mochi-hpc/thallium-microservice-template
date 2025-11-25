/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "DummyBackend.hpp"
#include <iostream>

ALPHA_REGISTER_BACKEND(dummy, DummyResource);

DummyResource::DummyResource(thallium::engine engine, const json& config)
: m_engine(std::move(engine)),
  m_config(config) {

}

std::string DummyResource::getConfig() const {
    return m_config.dump();
}

alpha::Result<int32_t> DummyResource::computeSum(int32_t x, int32_t y) {
    alpha::Result<int32_t> result;
    result.value() = x + y;
    return result;
}

std::unique_ptr<alpha::ResourceInterface> DummyResource::Create(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<alpha::ResourceInterface>(new DummyResource(engine, config));
}
