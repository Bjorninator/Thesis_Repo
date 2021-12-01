#include "lock_free_bst.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    // std::unique_ptr<BinarySearchTree> bst = std::make_unique<BST>();
    std::unique_ptr<BST> bst = std::make_unique<BST>();
    int a = 5;
    std::cout << a << "\n";
    int b = std::move(a);
    a = NULL;
    std::cout << b << "\n";
    std::cout << a << "\n";

    bst->insert(1000);
    bst->insert(1000);
    bst->insert(1000);
    bst->insert(1000);
    bst->insert(1000);
    bst->insert(1000);
    bst->insert(1000);
    bst->insert(1000);


    auto event = bst->extract(0);
    auto evtNode1 = std::move(event->event);
    bst->remove(event->value.value);
   

    event = bst->extract(0);
    evtNode1 = std::move(event->event);
    bst->remove(event->value.value);

    event = bst->extract(0);
    evtNode1 = std::move(event->event);
    bst->remove(event->value.value);
  

     event = bst->extract(0);
    evtNode1 = std::move(event->event);
    bst->remove(event->value.value);
   

    event = bst->extract(0);
    auto evtNode = std::move(event->value.value);
    bst->remove(event->value.value);
    std::cout << evtNode;

     event = bst->extract(0);
    evtNode = std::move(event->value.value);
    bst->remove(event->value.value);
    std::cout << evtNode;

    event = bst->extract(0);
    evtNode = std::move(event->value.value);
    bst->remove(event->value.value);
    std::cout << evtNode;

    event = bst->extract(0);
    evtNode = std::move(event->value.value);
    bst->remove(event->value.value);
    std::cout << evtNode;


     bst->remove(1000);
     bst->remove(1001);
     std::cout << bst->contains(1000);
     std::cout << bst->contains(1001);
     std::cout << bst->contains(1003);
    bst->insert(1000);
    bst->insert(1000);

    // bst->insert(1);
    // std::cout << bst->contains(1);
    return 0;
}
