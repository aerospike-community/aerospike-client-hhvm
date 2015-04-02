HHVM_EXTENSION(aerospike-hhvm ext_aerospike.cpp conversions.cpp)
HHVM_DEFINE(aerospike-hhvm -DAEROSPIKE_C_CHECK)
set(LIBAEROSPIKE, /usr/local)
target_link_libraries(aerospike ${LIBAEROSPIKE}/lib/libaerospike.so)
include_directories(${LIBAEROSPIKE}/include)
HHVM_SYSTEMLIB(aerospike-hhvm ext_aerospike.php)

