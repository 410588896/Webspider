CC 	= g++
THREADNUM	= 16
CFLAGS	= -g -DTHREAD_NUM=$(THREADNUM)
LIBS	= -L/usr/lib/mysql -lmysqlclient -lpthread

FOO := $(wildcard ./*.cpp)
BAR := $(basename $(FOO))
OBJ := $(addsuffix .o,$(BAR))
INC := $(wildcard ./*.h)

.cpp.o: 
	$(CC) -c $(CFLAGS) $(LIBS) $<

Webspider:$(OBJ)
	@echo "Compiling......"
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	@echo "Done......"

sinclude $(FOO:.cpp=.d)
%d:%cpp
	@echo "Create depend"
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

release:Webspider
	strip Webspider

clean:
	@echo "Cleaning......"
	rm -f *.o;rm -f *.d;rm -f Webspider;rm -f *~
	@echo "Done......"


