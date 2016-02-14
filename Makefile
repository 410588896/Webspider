CC 	= g++
CFLAGS	= -g
LIBS	= -L/usr/lib/mysql -lmysqlclient -lpthread

FOO := $(wildcard ./*.cpp)
BAR := $(basename $(FOO))
OBJ := $(addsuffix .o,$(BAR))

.cpp.o:
	$(CC) -c $(CFLAGS) $(LIBS) $<

Webspider:$(OBJ)
	@echo "Compiling......"
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	@echo "Done......"

release:Webspider
	strip Webspider

clean:
	@echo "Cleaning......"
	rm -f *.o;rm -f Webspider;rm -f *~
	@echo "Done......"


