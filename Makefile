CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

server: main.cpp ./http/http_conn.cpp ./dao/connection_pool.cpp  
	$(CXX) -o server  $^ $(CXXFLAGS) -pthread -Wall `mysql_config --cflags --libs`

clean:
	rm  -r server