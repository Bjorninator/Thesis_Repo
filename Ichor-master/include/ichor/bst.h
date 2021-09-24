#ifndef BST_H_
#define BST_H_

#include <memory>
#include <ichor/interfaces/IFrameworkLogger.h>
#include <ichor/Service.h>
#include <ichor/LifecycleManager.h>
#include <ichor/Events.h>
#include <ichor/Callbacks.h>
#include <ichor/Filter.h>
#include <ichor/DependencyRegistrations.h>
#include <ichor/stl/RealtimeMutex.h>
#include <ichor/stl/RealtimeReadWriteMutex.h>
#include <ichor/stl/ConditionVariable.h>
#include <ichor/stl/ConditionVariableAny.h>

class BinarySearchTree {
public:
    
    BinarySearchTree() : size(0) {};
    ~BinarySearchTree() = default;

    struct node;
    struct val;
    virtual bool insert(std::unique_ptr<Ichor::Event, Ichor::Deleter> x);
    virtual bool remove(int x);
    virtual bool contains(int x);
    virtual node* extract(int x);

    //// returns the in-order traversal, stores the number of nodes in size
    //virtual int* in_order_traversal(int* size) = 0;
protected:
    int size;
};

#endif
