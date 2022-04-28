Include_Dir := Include

CPPFILES := $(foreach dir,Source,$(wildcard $(dir)/*.cpp))
OBJECTS = $(patsubst Source/%.cpp, %.o, $(CPPFILES))

all: RARCExt

RARCExt: Main.cpp RARCExt.a
	g++ $^ -o $@

RARCExt.a: $(OBJECTS)
	ar src $@ $^
	rm $^

$(OBJECTS): $(CPPFILES)
	g++ -c $^ -Os -I $(Include_Dir)