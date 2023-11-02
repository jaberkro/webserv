# webserv
This is a HTTP server built from scratch in C++, using python for the CGI and HTML with CSS for the static webpages. Please note: kqueue() is used for I/O multiplexing, therefore this webserver only runs on MacOS.

Download:
```
git clone https://github.com/jaberkro/webserv.git
```

Move inside the webserv folder:
```
cd webserv
```

Compilation:
```
make
```

Execution:
```
./webserv <your_configuration_file>.conf
```
