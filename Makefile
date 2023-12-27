CXX = g++
RM = rm -f
MKDIR = mkdir -p
RMDIR = rmdir -p
CPPFLAGS = -g -c -std=c++17
LDFLAGS = -g -pthread -lGL -lX11 -lpng
DCFLAGS = -g -std=c++17 -pthread -lGL -lX11 -lpng

PGE_OBJ = pge.o
VECTOR_OBJ = vector.o
NETWORK_OBJ = $(VECTOR_OBJ) network.o
ENGINE_OBJ = $(PGE_OBJ) $(VECTOR_OBJ) objects.o
SOURCE = src
TARGET = bin
DIRECTORIES = $(TARGET)

all: gol fire fireworks lightning maze engine

maze: $(DIRECTORIES) $(PGE_OBJ) $(NETWORK_OBJ)
	$(CXX) $(SOURCE)/$@.cpp -o $@.o $(CPPFLAGS)
	$(CXX) $(PGE_OBJ) $(NETWORK_OBJ) $@.o -o $(TARGET)/$@ $(LDFLAGS)

gol: $(DIRECTORIES) $(PGE_OBJ)
	$(CXX) $(SOURCE)/$@.cpp -o $@.o $(CPPFLAGS)
	$(CXX) $(PGE_OBJ) $@.o -o $(TARGET)/$@ $(LDFLAGS)

fire lightning fireworks: $(PGE_OBJ) $(VECTOR_OBJ)
	$(CXX) $(SOURCE)/$@.cpp -o $@.o $(CPPFLAGS)
	$(CXX) $(PGE_OBJ) $(VECTOR_OBJ) $@.o -o $(TARGET)/$@ $(LDFLAGS)

engine: $(DIRECTORIES) $(ENGINE_OBJ)
	$(CXX) $(SOURCE)/$@.cpp -o $@.o $(CPPFLAGS)
	$(CXX) $(ENGINE_OBJ) $@.o -o $(TARGET)/$@ $(LDFLAGS)

wolf: $(DIRECTORIES) $(PGE_OBJ)
	$(CXX) $(SOURCE)/$@.cpp -o $@.o $(CPPFLAGS)
	$(CXX) $(PGE_OBJ) $@.o -o $(TARGET)/$@ $(LDFLAGS)

%.o: $(SOURCE)/%.cpp $(DIRECTORIES)
	$(CXX) $< -o $@ $(CPPFLAGS)

$(DIRECTORIES):
	$(MKDIR) $(DIRECTORIES)

clean: $(DIRECTORIES)
	$(RM) *.o
	$(RM) $(TARGET)/*
	$(RMDIR) $(DIRECTORIES)

