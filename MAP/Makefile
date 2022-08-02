TARGET ?= map
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.s)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LD_DIRS := 
LD_FLAGS := $(addprefix -L,$(LD_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -std=c++17 

$(TARGET): $(OBJS)
	gcc -I ./src/include -c ./src/mongoose.c -o ./src/mongoose.o
	g++ $(LD_FLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS) ./src/mongoose.o

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS) ./src/mongoose.o

-include $(DEPS)