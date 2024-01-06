SERVER_FILE="main_new.cpp"
# SERVER_FILE="main_v1.cpp"
# SERVER_FILE="main.cpp"
g++ -Wall -march=native -O3 ${SERVER_FILE} -lfcgi++ -lfcgi -lcrypto -g -o hello_world
echo -e "======runing server on port 80====="
spawn-fcgi -p 8000 -n hello_world my_response.xml.gz
