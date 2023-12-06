.PHONY: clean

L1TMLDemo_v1.so: L1TMLDemo_emulator_v1.o L1TMLDemo_v1.o
	$(CXX) $(CXXFLAGS) $(LD_FLAGS) -shared $^ -o $@

%.o: NN/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm L1TMLDemo_emulator_v1.o L1TMLDemo_v1.o L1TMLDemo_v1.so
