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
        public function put(array $key, array $rec, int $ttl=0, mixed $options = NULL): int;
    <<__Native>>
        public function get(array $key, mixed& $rec, mixed $filter = NULL, mixed $options = NULL): int;
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

    public function touch(array $key, int $ttl = 0/*, int $generation = 0*/, mixed $options = NULL): int {
        $returned = NULL;
        $operations = array(array("op" => self::OPERATOR_TOUCH, "metadata" => array("ttl" => $ttl/*, "generation" => $generation*/)));
        return $this->operate($key, $operations, $returned, $options);
    }
}

