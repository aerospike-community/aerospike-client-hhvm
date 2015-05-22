# Test Aerospike HHVM client using RESTful web-services in slim and nginx/apache2

1. Install apache2 + hhvm and configure them following these instructions:
   http://blakepetersen.io/how-to-set-up-and-configure-hhvm-on-ubuntu-14-04/

   OR

   Install nginx + hhvm and configure them following these instructions:
   http://fideloper.com/hhvm-nginx-laravel

2. Install slim framework and test a helloworld slim app following these
   instructions:
    https://www.digitalocean.com/community/tutorials/how-to-install-and-configure-slim-framework-on-ubuntu-14-04
    
3. Install httperf

4. Edit index.php to change server IP, port, Slim installation path etc. according to
   your set up.

5. Edit test-rest-apis.sh to change num-conn and rate for the hhtperf testing.

6. To run a simple sequential test of all REST APIs, run:

   $ sh test-rest-apis.sh

7. To run a concurrent version of the test, invoking multiple background
   processes of the sequential test, run:

   $ sh concurrent-test.sh [number of processes]

