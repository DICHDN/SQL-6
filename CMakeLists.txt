cmake_minimum_required (VERSION 3.20)

set(PostgreSQL_ADDITIONAL_VERSIONS "16")

project(CPPSQL6)

add_subdirectory("C:\\Users\\dichd\\YandexDisk\\Study\\Postgres-SQL\\WH6-2\\libpqxx-7" CPPSQL6-build)

add_executable(CPPSQL6 main.cpp)

target_compile_features(CPPSQL6 PRIVATE cxx_std_20)  

target_link_libraries(CPPSQL6 PRIVATE pqxx)  

