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
        public function errorno(): int;
    <<__Native>>
        public function error(): string;

    public function initKey(String $ns, String $set, mixed $key, bool $digest=false) {
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
}

