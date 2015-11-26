<?hh
<<__NativeData("Aerospike")>>
class Aerospike {
    <<__Native>>
        public function __construct(array $config, bool $persistent_connection = true, mixed $options = NULL): void;
    <<__Native>>
        public function isConnected(): bool;
    <<__Native>>
        public function close(): int;
    <<__Native>>
        public function reconnect(): int;
    <<__Native>>
        public static function setSerializer(mixed $callback = NULL): bool;
    <<__Native>>
        public static function setDeserializer(mixed $callback = NULL): bool;
    <<__Native>>
        public function put(array $key, array $rec, int $ttl=0, mixed $options = NULL): int;
    <<__Native>>
        public function get(array $key, mixed& $rec, mixed $filter = NULL, mixed $options = NULL): int;
    <<__Native>>
        public function dropIndex(mixed $ns, mixed $name, mixed $options = NULL): int;
    <<__Native>>
        public function addIndex(mixed $ns, mixed $set, mixed $bin, mixed $name, mixed $index_type, mixed $data_type, mixed $options = NULL): int;
    <<__Native>>
        public function getMany(array $keys, mixed& $records, mixed $filter = NULL, mixed $options = NULL): int;
    <<__Native>>
        public function operate(array $key, array $operations, mixed& $returned = NULL, mixed $options = NULL): int;
    <<__Native>>
        public function remove(array $key, mixed $options = NULL): int;
    <<__Native>>
        public function removeBin(array $key, array $bins, mixed $options = NULL): int;
    <<__Native>>
        public function exists(array $key, mixed& $metadata, mixed $options = NULL): int;
    <<__Native>>
        public function existsMany(array $keys, mixed& $metadata, mixed $options = NULL): int;
    <<__Native>>
        public function getKeyDigest(mixed $ns, mixed $set, mixed $key): string;
    <<__Native>>
        public function register(mixed $path, mixed $module, mixed $language = Aerospike::UDF_TYPE_LUA, mixed $options = NULL): int;
    <<__Native>>
        public function deregister(mixed $module, mixed $options = NULL): int;
    <<__Native>>
        public function getRegistered(mixed $module, mixed& $code, mixed $language = Aerospike::UDF_TYPE_LUA, mixed $options = NULL): int;
    <<__Native>>
        public function listRegistered(mixed& $modules, mixed $language = Aerospike::UDF_TYPE_LUA, mixed $options = NULL): int;
    <<__Native>>
        public function apply(array $key, mixed $module, mixed $function, mixed $args = NULL, mixed &$returned = NULL, mixed $options = NULL): int;
    <<__Native>>
        public function scan(mixed $ns, mixed $set, mixed $function, mixed $bins = NULL, mixed $options = NULL): int;
    <<__Native>>
        public function scanApply(mixed $ns, mixed $set, mixed $module, mixed $function, mixed $args, mixed &$scan_id, mixed $options = NULL): int;
    <<__Native>>
        public function scanInfo(mixed $scan_id, mixed &$scan_info, mixed $options = NULL): int;
    <<__Native>>
        public static function predicateEquals(mixed $ns, mixed $value): mixed;
    <<__Native>>
        public static function predicateContains(mixed $ns, mixed $index_type, mixed $value): mixed;
    <<__Native>>
        public static function predicateBetween(mixed $ns, mixed $min, mixed $max): mixed;
    <<__Native>>
        public static function predicateRange(mixed $ns, mixed $index_type, mixed $min, mixed $max): mixed;
    <<__Native>>
        public function query(mixed $ns, mixed $set, mixed $where, mixed $function, mixed $select = NULL, mixed $options = NULL): int;
    <<__Native>>
        public function aggregate(mixed $ns, mixed $set, mixed $where, mixed $module, mixed $function, mixed $args, mixed &$result, mixed $options = NULL): int;
    <<__Native>>
        public function errorno(): int;
    <<__Native>>
        public function error(): string;

    public function initKey(mixed $ns, mixed $set, mixed $key, bool $digest = false) {
        if ((!is_int($ns) && !is_string($ns)) ||
            (!is_int($set) && !is_string($set)) ||
            (!is_int($key) && !is_string($key))) {
            return NULL;
        }
        if ($digest) {
            if (strlen((string) $key) <= self::DIGEST_VALUE_SIZE) {
                return array("ns" => $ns, "set" => $set, "digest" => $key);
            } else {
                return NULL;
            }
        }
        return array("ns" => (string) $ns, "set" => (string) $set, "key" => $key);
    }

    public function append(array $key, string $bin, mixed $value, mixed $options = NULL): int {
        $returned = NULL;
        if (!is_int($value) && !is_string($value)) {
            return NULL;
        }
        if (is_int($value)) {
            $value = (string)$value;
        }
        $operations = array(array("op" => self::OPERATOR_APPEND, "bin" => $bin, "val" => $value));
        return $this->operate($key, $operations, $returned, $options);
    }

    public function prepend(array $key, string $bin, mixed $value, mixed $options = NULL): int {
        $returned = NULL;
        if (!is_int($value) && !is_string($value)) {
            return NULL;
        }
        if (is_int($value)) {
            $value = (string)$value;
        }
        $operations = array(array("op" => self::OPERATOR_PREPEND, "bin" => $bin, "val" => $value));
        return $this->operate($key, $operations, $returned, $options);
    }

    public function increment(array $key, string $bin, int $offset, mixed $options = NULL): int {
        $returned = NULL;
        $operations = array(array("op" => self::OPERATOR_INCR, "bin" => $bin, "val" => $offset));
        return $this->operate($key, $operations, $returned, $options);
    }

    public function touch(array $key, int $ttl = 0, mixed $options = NULL): int {
        $returned = NULL;
        $operations = array(array("op" => self::OPERATOR_TOUCH, "ttl" => $ttl));
        return $this->operate($key, $operations, $returned, $options);
    }
}

