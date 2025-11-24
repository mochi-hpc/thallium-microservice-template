add_library (_pyalpha_client MODULE py-alpha-client.cpp)
target_link_libraries (_pyalpha_client PRIVATE pybind11::module alpha::client PRIVATE coverage_config)
pybind11_extension (_pyalpha_client)
pybind11_strip (_pyalpha_client)

add_library (_pyalpha_server MODULE py-alpha-server.cpp)
target_link_libraries (_pyalpha_server PRIVATE pybind11::module alpha::server PRIVATE coverage_config)
pybind11_extension (_pyalpha_server)
pybind11_strip (_pyalpha_server)

set (PY_ALPHA_TARGETS _pyalpha_client _pyalpha_server)

set (PY_VERSION ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR})

install (TARGETS ${PY_ALPHA_TARGETS}
         EXPORT alpha-targets
         ARCHIVE DESTINATION lib/python${PY_VERSION}/site-packages
         LIBRARY DESTINATION lib/python${PY_VERSION}/site-packages)

install (DIRECTORY mochi/alpha
         DESTINATION lib/python${PY_VERSION}/site-packages/mochi)
