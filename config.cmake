HHVM_EXTENSION(aerospike ext_aerospike.cpp conversions.cpp)
HHVM_DEFINE(aerospike -DAEROSPIKE_C_CHECK)
set(LIBAEROSPIKE, /usr/local)
find_library(aerospike
    NAME aerospike
    PATHS LIBAEROSPIKE/lib)
target_link_libraries(aerospike ${aerospike})
include_directories(LIBAEROSPIKE/include)
HHVM_SYSTEMLIB(aerospike ext_aerospike.php)

