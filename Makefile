
CC		:= g++
CXXFLAGS:= -g -Wall -pedantic -Wno-long-long -Werror
LIBS	:= -I$(SYSTEMC_HOME)/include -I$(UVM_SYSTEMC_HOME)/include 
LD_LIBS	:= -lsystemc -Wl,-rpath=$(SYSTEMC_HOME)/lib-linux64 $(UVM_SYSTEMC_HOME)/lib-linux64/libuvm-systemc.so -Wl,-rpath=$(UVM_SYSTEMC_HOME)/lib-linux64

TARGET 	= sc_main
SRCS 	= sc_main.cpp
OBJS 	= $(SRCS:.cpp=.o)

all: clean $(TARGET) run

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LD_LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run:
	./$(TARGET)


# g++  $(CXXFLAGS)  -L$(SYSTEMC_HOME)/lib-linux64 -Wl,-rpath=$(SYSTEMC_HOME)/lib-linux64	\
# 					-L$(UVM_SYSTEMC_HOME)/lib-linux64/libuvm-systemc.so -Wl,-rpath=$(UVM_SYSTEMC_HOME)/lib-linux64 \
# 					-L$(UVM_SYSTEMC_HOME)/lib-linux64 -lsystemc -lm -o main.x main.o
# -I$(UVM_SYSTEMC_HOME)/examples/uvmsc -I$(UVM_SYSTEMC_HOME)/src

# build:
# 	g++  $(CXXFLAGS) $(LIBS) -c sc_main.cpp -o main.o
# # libtool --mode=link g++ $(CXXFLAGS)  -o main.x main.o $(UVM_SYSTEMC_HOME)/lib-linux64/libuvm-systemc.la  -lsystemc
# 	g++  $(CXXFLAGS) -o main.x main.o $(LD_LIBS)
