#include "single_threaded_tests.h"

#include <vector>
#include <unordered_set>
#include <algorithm>

namespace SingleThreaded {

void test_all() {
    test_single_add();
    test_triple_add();
    test_single_add_remove();
}

// Add a single value, and check that it was added.
bool test_single_add() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(1));
    EXPECT(bst->contains(1));
    EXIT_TEST;
}


// Add three values, check that they were all added successfully.
bool test_triple_add() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(1));
    EXPECT(bst->insert(0));
    EXPECT(bst->insert(2));
    EXPECT(bst->contains(1));
    EXPECT(bst->contains(0));
    EXPECT(bst->contains(2));
    EXIT_TEST;
}


// Add a value, check it exists, and then remove it, repeatedly. This is mostly
// to check that the root add / remove is working correctly.
bool test_single_add_remove() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    for (int i = -10; i < 10; ++i) {
        EXPECT(bst->insert(i));
        EXPECT(bst->contains(i));
        EXPECT(bst->remove(i));
        EXPECT_NOT(bst->contains(i));
    }
    EXIT_TEST;
}

} // namespace SingleThreaded
