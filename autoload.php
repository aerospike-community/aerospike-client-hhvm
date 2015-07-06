<?php
/* 
 * You can include this file and it will handle registering the
 * autoloaders.
 */
$autoloaders = spl_autoload_functions();
if (!is_array($autoloaders) || !array_key_exists('Aerospike\\Bytes', $autoloaders)) {
    spl_autoload_register(function ($class_name) {
        if ($class_name == 'Aerospike\Bytes') {
            require __DIR__. '/src/Bytes.php';
        }
    });
}

?>
