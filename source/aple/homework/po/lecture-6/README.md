## Install the Environment

 sudo apt-get install libfcgi-dev spawn-fcgi nginx curl make g++ libssl-dev


## Config Nginx

 sudo mv /etc/nginx/nginx.conf /etc/nginx/old_nginx.conf.bk
 
 sudo ln -s `pwd`/nginx.conf /etc/nginx/nginx.conf
 
 sudo /etc/init.d/nginx restart
 
 (remove the -n switch from the start executable if you want it to run in the background)

## Run server

### C app as a server
```
bash server_c.sh
```

Code Ref: https://github.com/homer6/fastcgi 

### python3 script as a server
```
bash server_py.sh
```

### Benchmarks/Testing
------------------

 curl --data "hello" http://localhost:80/

 ab -n 2000 -c 10 http://localhost:80/

 httperf --client=0/1 --server=localhost --port=81 --uri=/ --send-buffer=4096 --recv-buffer=16384 --num-conns=10 --num-calls=2000

