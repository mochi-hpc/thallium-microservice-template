add_executable (ClientTest ClientTest.cpp)
target_link_libraries (ClientTest PRIVATE Catch2::Catch2WithMain alpha::server alpha::client)
add_test (NAME ClientTest COMMAND ./ClientTest)

add_executable (ResourceTest ResourceTest.cpp)
target_link_libraries (ResourceTest PRIVATE Catch2::Catch2WithMain alpha::server alpha::client)
add_test (NAME ResourceTest COMMAND ./ResourceTest)
