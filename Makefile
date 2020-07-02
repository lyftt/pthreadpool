DEPENDS= main.o
CFLAGS=
EXENAME=pthreadPool
LDFLAGS=-lpthread -std=c++11

all:$(DEPENDS)
	g++ -o $(EXENAME) $(DEPENDS) $(LDFLAGS)

$(DEPENDS):%.o:%.cpp
	g++ -c $(CFLAGS) $< -o $@

.PHONY : clean

clean:
	rm ./*.o; rm pthreadPool;

