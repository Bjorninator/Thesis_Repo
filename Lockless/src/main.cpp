#include "single_threaded_tests.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    std::cout << "Starting single threaded tests...\n";
    SingleThreaded::test_all();
    std::cout << "Done!";
    return 0;
}
