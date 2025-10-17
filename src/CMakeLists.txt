aux_source_directory(. SRC_LIST)
add_library(mprpc ${SRC_LIST})
add_definitions(-DTHREADED)
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
