<?hh
<<__NativeData("Aerospike")>>
class Aerospike {
    <<__Native>>
        public function __construct(array $config, mixed $options = NULL): void;
    <<__Native>>
        public function isConnected(): bool;
    <<__Native>>
        public function close(): int;
    <<__Native>>
        public function put(array $key, array $rec, int $ttl=0, mixed $options = NULL): int;
    <<__Native>>
        public function get(array $key, mixed& $rec, mixed $options = NULL): int;
    <<__Native>>
        public function operate(array $key, array $operations, mixed& $returned = NULL): int;
    <<__Native>>
        public function errorno(): int;
    <<__Native>>
        public function error(): string;

    public function initKey(String $ns, String $set, mixed $key, bool $digest = false) {
        if (!is_int($key) && !is_string($key)) {
            return NULL;
        }
        if ($digest) {
            if (strlen((string) $key) <= self::DIGEST_VALUE_SIZE) {
                return array("ns" => $ns, "set" => $set, "digest" => $key);
            } else {
                return NULL;
            }
        }
        return array("ns" => $ns, "set" => $set, "key" => $key);
    }

    public function append(array $key, string $bin, string $value): int {
        $operations = array(array("op" => self::OPERATOR_APPEND, "bin" => $bin, "val" => $value));
        return $this->operate($key, $operations);
    }

    public function prepend(array $key, string $bin, string $value): int {
        $operations = array(array("op" => self::OPERATOR_PREPEND, "bin" => $bin, "val" => $value));
        return $this->operate($key, $operations);
    }

    public function increment(array $key, string $bin, int $offset): int {
        $operations = array(array("op" => self::OPERATOR_INCR, "bin" => $bin, "val" => $offset));
        return $this->operate($key, $operations);
    }

    public function touch(array $key, int $ttl = 0/*, int $generation = 0*/): int {
        $operations = array(array("op" => self::OPERATOR_TOUCH, "metadata" => array("ttl" => $ttl/*, "generation" => $generation*/)));
        return $this->operate($key, $operations);
    }
}

