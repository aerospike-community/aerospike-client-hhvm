#ifndef __CLASS_CONSTANTS_H__
#define __CLASS_CONSTANTS_H__

#include "constants.h"

namespace HPHP {
    /*
     *******************************************************************************************************
     * Instance of Mapper of constant number to constant name string for Aerospike extension constants.
     *******************************************************************************************************
     */
    aerospike_constants extension_constants[] = {
        /*
         * Aerospike Error/Status Code Constants wrappers for C client's constants
         */
        { AEROSPIKE_ERR_INVALID_HOST            ,   "ERR_INVALID_HOST"                  },
        { AEROSPIKE_ERR_PARAM                   ,   "ERR_PARAM"                         },
        { AEROSPIKE_ERR_CLIENT                  ,   "ERR_CLIENT"                        },
        { AEROSPIKE_OK                          ,   "OK"                                },
        { AEROSPIKE_ERR_SERVER                  ,   "ERR_SERVER"                        },
        { AEROSPIKE_ERR_RECORD_NOT_FOUND        ,   "ERR_RECORD_NOT_FOUND"              },
        { AEROSPIKE_ERR_RECORD_GENERATION       ,   "ERR_RECORD_GENERATION"             },
        { AEROSPIKE_ERR_REQUEST_INVALID         ,   "ERR_REQUEST_INVALID"               },
        { AEROSPIKE_ERR_RECORD_EXISTS           ,   "ERR_RECORD_EXISTS"                 },
        { AEROSPIKE_ERR_BIN_EXISTS              ,   "ERR_BIN_EXISTS"                    },
        { AEROSPIKE_ERR_CLUSTER_CHANGE          ,   "ERR_CLUSTER_CHANGE"                },
        { AEROSPIKE_ERR_SERVER_FULL             ,   "ERR_SERVER_FULL"                   },
        { AEROSPIKE_ERR_TIMEOUT                 ,   "ERR_TIMEOUT"                       },
        { AEROSPIKE_ERR_NO_XDR                  ,   "ERR_NO_XDR"                        },
        { AEROSPIKE_ERR_CLUSTER                 ,   "ERR_CLUSTER"                       },
        { AEROSPIKE_ERR_BIN_INCOMPATIBLE_TYPE   ,   "ERR_BIN_INCOMPATIBLE_TYPE"         },
        { AEROSPIKE_ERR_RECORD_TOO_BIG          ,   "ERR_RECORD_TOO_BIG"                },
        { AEROSPIKE_ERR_RECORD_BUSY             ,   "ERR_RECORD_BUSY"                   },
        { AEROSPIKE_ERR_SCAN_ABORTED            ,   "ERR_SCAN_ABORTED"                  },
        { AEROSPIKE_ERR_UNSUPPORTED_FEATURE     ,   "ERR_UNSUPPORTED_FEATURE"           },
        { AEROSPIKE_ERR_BIN_NOT_FOUND           ,   "ERR_BIN_NOT_FOUND"                 },
        { AEROSPIKE_ERR_DEVICE_OVERLOAD         ,   "ERR_DEVICE_OVERLOAD"               },
        { AEROSPIKE_ERR_RECORD_KEY_MISMATCH     ,   "ERR_RECORD_KEY_MISMATCH"           },
        { AEROSPIKE_ERR_NAMESPACE_NOT_FOUND     ,   "ERR_NAMESPACE_NOT_FOUND"           },
        { AEROSPIKE_ERR_BIN_NAME                ,   "ERR_BIN_NAME"                      },
        { AEROSPIKE_QUERY_END                   ,   "ERR_QUERY_END"                     },
        { AEROSPIKE_ERR_UDF                     ,   "ERR_UDF"                           },
        { AEROSPIKE_ERR_LARGE_ITEM_NOT_FOUND    ,   "ERR_LARGE_ITEM_NOT_FOUND"          },
        { AEROSPIKE_ERR_INDEX_FOUND             ,   "ERR_INDEX_FOUND"                   },
        { AEROSPIKE_ERR_INDEX_NOT_FOUND         ,   "ERR_INDEX_NOT_FOUND"               },
        { AEROSPIKE_ERR_INDEX_OOM               ,   "ERR_INDEX_OOM"                     },
        { AEROSPIKE_ERR_INDEX_NOT_READABLE      ,   "ERR_INDEX_NOT_READABLE"            },
        { AEROSPIKE_ERR_INDEX                   ,   "ERR_INDEX"                         },
        { AEROSPIKE_ERR_INDEX_NAME_MAXLEN       ,   "ERR_INDEX_NAME_MAXLEN"             },
        { AEROSPIKE_ERR_INDEX_MAXCOUNT          ,   "ERR_INDEX_MAXCOUNT"                },
        { AEROSPIKE_ERR_QUERY_ABORTED           ,   "ERR_QUERY_ABORTED"                 },
        { AEROSPIKE_ERR_QUERY_QUEUE_FULL        ,   "ERR_QUERY_QUEUE_FULL"              },
        { AEROSPIKE_ERR_QUERY_TIMEOUT           ,   "ERR_QUERY_TIMEOUT"                 },
        { AEROSPIKE_ERR_QUERY                   ,   "ERR_QUERY"                         },
        { AEROSPIKE_ERR_UDF_NOT_FOUND           ,   "ERR_UDF_NOT_FOUND"                 },
        { AEROSPIKE_ERR_LUA_FILE_NOT_FOUND      ,   "ERR_LUA_FILE_NOT_FOUND"            },
        { AS_DIGEST_VALUE_SIZE                  ,   "DIGEST_VALUE_SIZE"              },
        /*
         * PHP Client Specific Constants
         */
        { OPT_CONNECT_TIMEOUT                   ,   "OPT_CONNECT_TIMEOUT"               },
        { OPT_READ_TIMEOUT                      ,   "OPT_READ_TIMEOUT"                  },
        { OPT_WRITE_TIMEOUT                     ,   "OPT_WRITE_TIMEOUT"                 },
        { OPT_POLICY_RETRY                      ,   "OPT_POLICY_RETRY"                  },
        { OPT_POLICY_EXISTS                     ,   "OPT_POLICY_EXISTS"                 },
        { OPT_POLICY_KEY                        ,   "OPT_POLICY_KEY"                    },
        { OPT_SERIALIZER                        ,   "OPT_SERIALIZER"                    },
        { OPT_SCAN_PRIORITY                     ,   "OPT_SCAN_PRIORITY"                 },
        { OPT_SCAN_PERCENTAGE                   ,   "OPT_SCAN_PERCENTAGE"               },
        { OPT_SCAN_CONCURRENTLY                 ,   "OPT_SCAN_CONCURRENTLY"             },
        { OPT_SCAN_NOBINS                       ,   "OPT_SCAN_NOBINS"                   },
        { OPT_POLICY_GEN                        ,   "OPT_POLICY_GEN"                    },
        { OPT_POLICY_REPLICA                    ,   "OPT_POLICY_REPLICA"                },
        { OPT_POLICY_CONSISTENCY                ,   "OPT_POLICY_CONSISTENCY"            },
        { OPT_POLICY_COMMIT_LEVEL               ,   "OPT_POLICY_COMMIT_LEVEL"           },
        { AS_POLICY_RETRY_NONE                  ,   "POLICY_RETRY_NONE"                 },
        { AS_POLICY_RETRY_ONCE                  ,   "POLICY_RETRY_ONCE"                 },
        { AS_POLICY_EXISTS_IGNORE               ,   "POLICY_EXISTS_IGNORE"              },
        { AS_POLICY_EXISTS_CREATE               ,   "POLICY_EXISTS_CREATE"              },
        { AS_POLICY_EXISTS_UPDATE               ,   "POLICY_EXISTS_UPDATE"              },
        { AS_POLICY_EXISTS_REPLACE              ,   "POLICY_EXISTS_REPLACE"             },
        { AS_POLICY_EXISTS_CREATE_OR_REPLACE    ,   "POLICY_EXISTS_CREATE_OR_REPLACE"   },
        { SERIALIZER_NONE                       ,   "SERIALIZER_NONE"                   },
        { SERIALIZER_PHP                        ,   "SERIALIZER_PHP"                    },
        { SERIALIZER_JSON                       ,   "SERIALIZER_JSON"                   },
        { SERIALIZER_USER                       ,   "SERIALIZER_USER"                   },
        { AS_UDF_TYPE_LUA                       ,   "UDF_TYPE_LUA"                      },
        { AS_SCAN_PRIORITY_AUTO                 ,   "SCAN_PRIORITY_AUTO"                },
        { AS_SCAN_PRIORITY_LOW                  ,   "SCAN_PRORITY_LOW"                  },
        { AS_SCAN_PRIORITY_MEDIUM               ,   "SCAN_PRIORITY_MEDIUM"              },
        { AS_SCAN_PRIORITY_HIGH                 ,   "SCAN_PRIORITY_HIGH"                },
        { AS_SCAN_STATUS_UNDEF                  ,   "SCAN_STATUS_UNDEF"                 },
        { AS_SCAN_STATUS_INPROGRESS             ,   "SCAN_STATUS_INPROGRESS"            },
        { AS_SCAN_STATUS_ABORTED                ,   "SCAN_STATUS_ABORTED"               },
        { AS_SCAN_STATUS_COMPLETED              ,   "SCAN_STATUS_COMPLETED"             },
        { AS_POLICY_KEY_DIGEST                  ,   "POLICY_KEY_DIGEST"                 },
        { AS_POLICY_KEY_SEND                    ,   "POLICY_KEY_SEND"                   },
        { AS_POLICY_GEN_IGNORE                  ,   "POLICY_GEN_IGNORE"                 },
        { AS_POLICY_GEN_EQ                      ,   "POLICY_GEN_EQ"                     },
        { AS_POLICY_GEN_GT                      ,   "POLICY_GEN_GT"                     },
        { AS_POLICY_REPLICA_MASTER              ,   "POLICY_REPLICA_MASTER"             },
        { AS_POLICY_REPLICA_ANY                 ,   "POLICY_REPLICA_ANY"                },
        { AS_POLICY_CONSISTENCY_LEVEL_ONE       ,   "POLICY_CONSISTENCY_ONE"            },
        { AS_POLICY_CONSISTENCY_LEVEL_ALL       ,   "POLICY_CONSISTENCY_ALL"            },
        { AS_POLICY_COMMIT_LEVEL_ALL            ,   "POLICY_COMMIT_LEVEL_ALL"           },
        { AS_POLICY_COMMIT_LEVEL_MASTER         ,   "POLICY_COMMIT_LEVEL_MASTER"        },
        { AS_INDEX_STRING                       ,   "INDEX_STRING"                      },
        { AS_INDEX_NUMERIC                      ,   "INDEX_NUMERIC"                     },
        { AS_INDEX_TYPE_DEFAULT                 ,   "INDEX_TYPE_DEFAULT"                },
        { AS_INDEX_TYPE_LIST                    ,   "INDEX_TYPE_LIST"                   },
        { AS_INDEX_TYPE_MAPKEYS                 ,   "INDEX_TYPE_MAPKEYS"                },
        { AS_INDEX_TYPE_MAPVALUES               ,   "INDEX_TYPE_MAPVALUES"              },
        { AS_OPERATOR_WRITE                     ,   "OPERATOR_WRITE"                    },
        { AS_OPERATOR_READ                      ,   "OPERATOR_READ"                     },
        { AS_OPERATOR_INCR                      ,   "OPERATOR_INCR"                     },
        { AS_OPERATOR_PREPEND                   ,   "OPERATOR_PREPEND"                  },
        { AS_OPERATOR_APPEND                    ,   "OPERATOR_APPEND"                   },
        { AS_OPERATOR_TOUCH                     ,   "OPERATOR_TOUCH"                    }
};

#define EXTENSION_CONSTANTS_SIZE (sizeof(extension_constants)/sizeof(aerospike_constants))

} // namespace HPHP
#endif /* end of __CLASS_CONSTANTS_H__ */
