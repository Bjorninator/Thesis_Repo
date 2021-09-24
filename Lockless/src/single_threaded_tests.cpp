#include "single_threaded_tests.h"

#include <vector>
#include <unordered_set>
#include <algorithm>

namespace SingleThreaded {

void test_all() {
    test_single_add();
    test_triple_add();
    test_single_add_remove();
    test_bad_remove();
    test_repeated_add();
    test_nonexistent_remove();
    test_leaf_remove();
    test_single_child_remove();
    test_double_child_remove();
    test_single_child_remove_root();
    test_double_child_remove_root();
    test_double_child_remove_root_deep();
    test_root_remove_deep();
    test_middle_remove_deep();
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


// Attempt to remove a value multiple times.
bool test_bad_remove() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT_NOT(bst->remove(0));
    EXPECT_NOT(bst->remove(1));
    EXPECT(bst->insert(0));
    EXPECT(bst->remove(0));
    EXPECT_NOT(bst->remove(0));
    EXIT_TEST;
}


// Add a value multiple times, check it only gets added the first time.
bool test_repeated_add() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    
    EXPECT_NOT(bst->contains(1));
    EXPECT(bst->insert(1));
    EXPECT(bst->contains(1));
    for (int i = 0; i < 10; ++i) {
        EXPECT_NOT(bst->insert(1));
        EXPECT(bst->contains(1));
    }
    EXIT_TEST;
}


// Attempt to remove many values which don't exist.
bool test_nonexistent_remove() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    for (int i = -2; i < 10; ++i) {
        EXPECT_NOT(bst->remove(i));
    }
    EXIT_TEST;
}


// Attempt to remove a leaf node from the bst (that is not the root).
bool test_leaf_remove() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(1));
    EXPECT(bst->insert(0));

    EXPECT(bst->remove(0));

    EXPECT(bst->contains(1));
    EXPECT_NOT(bst->contains(0));
    EXIT_TEST;
}


// Attempt to remove a node with a single child
bool test_single_child_remove() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(0));
    EXPECT(bst->insert(-1));
    EXPECT(bst->insert(-2));

    EXPECT(bst->remove(-1));

    EXPECT(bst->contains(0));
    EXPECT_NOT(bst->contains(-1));
    EXPECT(bst->contains(-2));
    EXIT_TEST;
}


// Attempt to remove a node which has two children.
bool test_double_child_remove() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(0));
    EXPECT(bst->insert(2));
    EXPECT(bst->insert(1));
    EXPECT(bst->insert(3));

    EXPECT(bst->remove(2));

    EXPECT(bst->contains(0));
    EXPECT_NOT(bst->contains(2));
    EXPECT(bst->contains(1));
    EXPECT(bst->contains(3));
    EXIT_TEST;
}


// Remove the root node when it has a single child.
bool test_single_child_remove_root() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(0));
    EXPECT(bst->insert(1));

    EXPECT(bst->remove(0));

    EXPECT_NOT(bst->contains(0));
    EXPECT(bst->contains(1));
    EXIT_TEST;
}


// Remove the root when it has multiple children
bool test_double_child_remove_root() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(0));
    EXPECT(bst->insert(-1));
    EXPECT(bst->insert(2));

    EXPECT(bst->remove(0));
    
    EXPECT_NOT(bst->contains(0));
    EXPECT(bst->contains(-1));
    EXPECT(bst->contains(2));
    EXIT_TEST;
}

// Make the min of the nodes on the right go a few levels down.
bool test_double_child_remove_root_deep() {
    INIT_TEST;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    EXPECT(bst->insert(0));
    EXPECT(bst->insert(-1));
    EXPECT(bst->insert(10));
    EXPECT(bst->insert(20));
    EXPECT(bst->insert(5));
    EXPECT(bst->insert(8));

    EXPECT(bst->remove(0));

    EXPECT_NOT(bst->contains(0));
    EXPECT(bst->contains(-1));
    EXPECT(bst->contains(10));
    EXPECT(bst->contains(20));
    EXPECT(bst->contains(5));
    EXPECT(bst->contains(8));


    EXIT_TEST;
}

static bool ordered_helper(std::vector<int> insert, std::vector<int> remove) {
    bool passed = true;
    std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    for (const int elem : insert) {
        EXPECT(bst->insert(elem));
    }
    
    std::vector<int> removed;
    for (const int elem : remove) {
        EXPECT(bst->remove(elem));

        insert.erase(std::find(insert.begin(), insert.end(), elem));
        removed.push_back(elem);

        for (const int removed_elem : removed) {
            EXPECT_NOT(bst->contains(removed_elem));
        }

        for (const int inserted : insert) {
            EXPECT(bst->contains(inserted));
        }
    }
    return passed;
}


// Add a bunch of items and then remove them all, one by one. Each element will
// be the root when removed.
bool test_root_remove_deep() {
    INIT_TEST;
    std::vector<int> insert_order = {0, -1, 10, 20, 5, 8};
    std::vector<int> remove_order = {0, 5, 8, 10, 20, -1};
    EXPECT(ordered_helper(insert_order, remove_order));
    EXIT_TEST;
}


// Add a bunch of single child items, remove them from the middle rather than
// from the root.
bool test_middle_remove_deep() {
    INIT_TEST;
    std::vector<int> insert_order = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> remove_order = {2, 3, 4, 5, 6, 7, 8, 9, 1};
    EXPECT(ordered_helper(insert_order, remove_order));
    EXIT_TEST;
}


} // namespace SingleThreaded
