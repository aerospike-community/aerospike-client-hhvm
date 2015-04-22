<?hh
<<__NativeData("Aerospike")>>
class Aerospike {
    <<__Native>>
        public function __construct(array $config): void;
    <<__Native>>
        public function isConnected(): bool;
    <<__Native>>
        public function close(): int;
    <<__Native>>
        public function put(array $key, array $rec): int;
    <<__Native>>
        public function get(array $key, mixed& $rec): int;
    <<__Native>>
        public function errorno(): int;
    <<__Native>>
        public function error(): string;

    public function initKey(String $ns, String $set, mixed $key) {
        return array("ns" => $ns, "set" => $set, "key" => $key);
    }
}

