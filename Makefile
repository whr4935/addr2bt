TARGET:=addr2bt

CXX:=g++
CXXFLAGS := -std=c++11

SRCS:= $(wildcard *.cpp)
OBJS:=$(SRCS:.cpp=.o)
DEPS:= $(SRCS:.cpp=.dep)  
 
$(TARGET):$(OBJS)
	$(CXX) -o $@ $^
 
-include $(DEPS)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@ -MD -MF $(@:.o=.dep)

#.PHONY:$(TARGET)

clean:
	-rm -rf $(TARGET) $(DEPS) $(OBJS)
