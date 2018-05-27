cmake_minimum_required(VERSION 3.11.1)

project(NetworkJoin)

find_package(MPI REQUIRED)

set(PROJECT_LINK_LIBS -lboost_serialization -lboost_mpi)

set(SOURCE_DIR src)
set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/ioutil.cpp
    ${SOURCE_DIR}/util.cpp
    ${SOURCE_DIR}/MurmurHash3.cpp
    ${SOURCE_DIR}/mpiutil.cpp
)


include_directories(include)

add_executable(main ${SOURCES})

target_link_libraries(main ${PROJECT_LINK_LIBS} ${MPI_LIBRARIES})
