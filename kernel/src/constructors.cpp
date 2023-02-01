#include <constructors.hpp>

extern "C" void callConstructors() {
	
	for(constructor* i = &start_ctors; i < &end_ctors; i++) (*i)();
	
}
