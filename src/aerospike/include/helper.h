#ifndef __HELPER_H__
#define __HELPER_H__

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

extern "C" {
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/as_record.h"
}

namespace HPHP {
    extern as_status get_digest_from_php_key(const Array& php_key, as_key& key,
            char **digest_pp, as_error& error);
    extern as_status set_nil_bins(as_record *record_p, const Array& php_bins,
            as_error& error);
    extern as_status process_filter_bins(const Array& php_filter_bins,
            const char **filter, as_error& error);
    extern as_status aerospike_get_filtered_bins(const Array& php_filter_bins,
            aerospike *as_p, as_policy_read *read_policy_p, as_key& key,
            as_record **record_pp, as_error& error);
} // namespace HPHP
#endif /* end of __HELPER_H__ */
