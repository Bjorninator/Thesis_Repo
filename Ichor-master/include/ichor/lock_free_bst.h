#pragma once

#include <queue>
#include <stack>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <cassert>
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <csignal>
#include <ichor/Events.h>
// #include "bst.h"

// prevent false positives by TSAN
// See "ThreadSanitizer – data race detection in practice" by Serebryany et al. for more info: https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/35604.pdf
#if defined(__SANITIZE_THREAD__)
#define TSAN_ENABLED
#elif defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define TSAN_ENABLED
#endif
#endif

#ifdef TSAN_ENABLED
#define TSAN_ANNOTATE_HAPPENS_BEFORE(addr) \
    AnnotateHappensBefore(__FILE__, __LINE__, (void*)(addr))
#define TSAN_ANNOTATE_HAPPENS_AFTER(addr) \
    AnnotateHappensAfter(__FILE__, __LINE__, (void*)(addr))
extern "C" void AnnotateHappensBefore(const char* f, int l, void* addr);
extern "C" void AnnotateHappensAfter(const char* f, int l, void* addr);
#else
#define TSAN_ANNOTATE_HAPPENS_BEFORE(addr)
#define TSAN_ANNOTATE_HAPPENS_AFTER(addr)
#endif



#define PTR(x) (reinterpret_cast<node*>(x))
#define LNG(x) (reinterpret_cast<unsigned long>(x))
#define BL(x)  (LNG(x) != 0)

#define GET_ADDR(nodeptr) (PTR(LNG(nodeptr) & (~(TAG_BIT | FLAG_BIT))))

#define GET_LEFT(nodeptr) (GET_ADDR(nodeptr)->left)
#define GET_RIGHT(nodeptr) (GET_ADDR(nodeptr)->right)

#define GET_TAG(nodeptr) (BL(LNG(nodeptr) & TAG_BIT))
#define GET_FLAG(nodeptr) (BL(LNG(nodeptr) & FLAG_BIT))

#define TAGGED(nodeptr) (PTR(LNG(nodeptr) | TAG_BIT))
#define FLAGGED(nodeptr) (PTR(LNG(nodeptr) | FLAG_BIT))

#define UNTAGGED(nodeptr) (PTR(LNG(nodeptr) & (~TAG_BIT)))
#define UNFLAGGED(nodeptr) (PTR(LNG(nodeptr) & (~FLAG_BIT)))

#define TAG_BIT 1
#define FLAG_BIT 2

class BST {
public:

    bool insert(std::unique_ptr<Ichor::Event> x){
        int treeId = treeIdCounter.fetch_add(1, std::memory_order_acq_rel);
        // std::unique_ptr<BST> bst = std::make_unique<BST>();
        int id = x.get()->id;
        int priority = x.get()->priority;
        val v(priority * 10000, id);
       
        seek_record record;
        while (true) {
            record = seek(v);
            if (record.leaf->value == v) {
                if(priority == 1000){
                   // std::cout<<idCounter <<" waddap \n";
                    idCounter++;
                    v.value = v.value + idCounter; 
                    // std:: cout << v.value << "\n";
                }else{
                    v.value++;
                }
            } else {
                node* parent = record.parent;
                node* leaf = record.leaf;

                node* new_leaf = new node(v);
                new_leaf->event = std::move(x);

                val k = leaf->value;
                node* new_internal;
                if (k < v) {
                    new_internal = new node(v, leaf, new_leaf);
                } else {
                    new_internal = new node(k, new_leaf, leaf);
                }

                node** child_addr;
                if (v < parent->value) {
                    child_addr = &(parent->left);
                } else {
                    child_addr = &(parent->right);
                }
                
                TSAN_ANNOTATE_HAPPENS_BEFORE((void*)&(*new_internal));
                if (__sync_bool_compare_and_swap(child_addr, leaf, new_internal)){
                    return true;
                } else {
                    std::cout << "does this ever happen\n";
                    // TODO: cleanup
                    delete new_leaf;
                    delete new_internal;

                    node* addr = GET_ADDR(child_addr);
                    if (addr == leaf && (GET_TAG(child_addr) || GET_FLAG(child_addr))) {
                        cleanup(v, record);
                        std::cout << "get stuck here?\n";
                    }
                }
            }
        }
    }

    bool empty(){
        seek_record record = seek(0);
        if(record.leaf->value.value == 0) {return true;}
        return false;
    }

    void deallocate_start (){
        std::cout << "FREE IT\n";
        val v(1);
        seek_record record = seek(v);
        node* event = record.leaf;
      //  deallocate(event);
    }

    bool remove(int x) {
        val v(x);
        if(idCounter >= 10000000 ){idCounter = x - 10000000;}
        bool injecting = true;
        node* leaf = nullptr;
        while (true) {
            seek_record record = seek(v);
            node* parent = record.parent;
            node** child_addr;
            if (v < parent->value) {
                child_addr = &(parent->left);
            } else {
                child_addr = &(parent->right);
            }
            if (injecting) {
                leaf = record.leaf;
                if (leaf->value != v)
                    return false;
                TSAN_ANNOTATE_HAPPENS_AFTER((void*)&(*leaf));
                if (__sync_bool_compare_and_swap(child_addr, GET_ADDR(leaf), FLAGGED(UNTAGGED(leaf)))) {
                    delete GET_ADDR(leaf);

                    injecting = false;
                    if (cleanup(v, record)){
                        return true;
                    }
                }else {
                    node* addr = GET_ADDR(child_addr);
                    if (addr == leaf && (GET_TAG(child_addr) || GET_FLAG(child_addr))) {
                      
                        cleanup(v, record);
                    }
                    
                }
            
            } else if (cleanup(v, record)) {
                
                return true;
            }
        }

    }


    bool contains(int x) {
        val v(x);
        seek_record record = seek(v);
        return record.leaf->value == v;
    }

    std::vector<int> in_order_traversal() {
        std::vector<int> out;
        std::stack<node*> nodes;
        nodes.push(root);
        node* current_node;
        while(!nodes.empty()) {
            current_node = nodes.top(); nodes.pop();
            if (GET_ADDR(current_node->left) == nullptr) {
                if (!current_node->value.inf)
                    out.push_back(current_node->value.value);
            } else {
                nodes.push(current_node->right);
                nodes.push(current_node->left);
            }
        }
        return out;
    }


    BST() {
        val inf0(true, 0);
        val inf1(true, 1);
        val inf2(true, 2);
        node* R = new node(inf2);
        node* S = new node(inf1);
        R->left = S;
        R->right = new node(inf2);
        S->left = new node(inf0);
        S->right = new node(inf1);
        root = R;
    }

    ~BST() {
        std::queue<node*> nodes;
        nodes.push(GET_ADDR(root));
        node* current_node;
        while (!nodes.empty()) {
            current_node = nodes.front(); nodes.pop();
            if (current_node == nullptr) continue;
            nodes.push(GET_LEFT(current_node));
            nodes.push(GET_RIGHT(current_node));
            delete GET_ADDR(current_node);
        }
    }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    struct val {
        bool inf;
        int value;
        int ID;
        val (int value, int ID) : inf(false), value(value), ID(ID) {}
        val (bool inf, int value, int ID) : inf(inf), value(value), ID(ID) {}
        val (bool inf, int value) : inf(inf), value(value), ID(0) {}
        val (int value) : inf(false), value(value), ID(1) {}

        bool operator<(const val& rhs) {
            if (rhs.inf && !inf) return true;
            else if (inf && !rhs.inf) return false;
            else return (value < rhs.value);
        }
        bool operator==(const val& rhs) {
            return inf == rhs.inf && value == rhs.value;
        }
        bool operator!=(const val& rhs) {
            return !(*this == rhs);
        }
    };

    struct node {
        val value;
        std::unique_ptr<Ichor::Event> event; 
        node* left;
        node* right;

        node(val value) : value(value), event(nullptr), left(nullptr), right(nullptr) {}
        node(val value, node* left, node* right) : value(value), event(nullptr), left(left), right(right) {}

        // node(val value) : value(value), event(nullptr), left(nullptr), right(nullptr) {}
        // node(val value, uint64_t eventId, uint64_t originatingServiceId, uint64_t priority, Args&&... args) : value(value), left(nullptr), right(nullptr), event(std::unique_ptr<Ichor::Event, Ichor::Deleter>(new (_memResource->allocate(sizeof(EventT))) EventT(std::forward<uint64_t>(eventId), std::forward<uint64_t>(originatingServiceId), std::forward<uint64_t>(priority), std::forward<Args>(args)...), Ichor::Deleter{_memResource, sizeof(EventT)})) {}
        // node(val value, node* left, node* right) :
        //     value(value), event(nullptr), left(left), right(right) {}

        // node(val value) : value(value), left(nullptr), right(nullptr),
        // x(std::unique_ptr<Event, Deleter>(new (_memResource->allocate(sizeof(EventT))) EventT(std::forward<uint64_t>(eventId), std::forward<uint64_t>(originatingServiceId), std::forward<uint64_t>(priority), std::forward<Args>(args)...), Deleter{_memResource, sizeof(EventT)}))) {}
        // node(val value) : value(value), x(nullptr), left(nullptr), right(nullptr) {}
        // node(val value, node* left, node* right) : value(value), x(nullptr), left(left), right(right) {}
    };

    virtual node* extract(int x) {
        val v(x);
        seek_record record = seek(v);
        node* event = record.leaf;
        if(event->value.value == 0) {return nullptr;}
        TSAN_ANNOTATE_HAPPENS_AFTER((void*)&(*event)); 
        return event;
    }

    void deallocate (node* event) 
    {
        if(event==NULL)
            return;

        deallocate(event->right);
        deallocate(event->left);
        (event->event).reset();
        delete event;
    }

private:
    std::atomic<uint64_t> treeIdCounter{0};
    int idCounter{0};

    struct seek_record {
        node* ancestor;
        node* successor;
        node* parent;
        node* leaf;
    };

    node* root;

    seek_record seek(val value) {

        seek_record record;
        record.ancestor = root;
        node* S = root;
        record.successor = S;
        record.parent = S;
        record.leaf = GET_ADDR(GET_LEFT(S));

        node* parentField = record.parent;
        node* currentField = record.leaf;
        node* current = GET_ADDR(currentField);

        while (current != nullptr) {
            if (!GET_TAG(parentField)) {
                record.ancestor = record.parent;
                record.successor = record.leaf;
            }
            record.parent = record.leaf;
            record.leaf = current;

            parentField = currentField;
            if (value < current->value) {
                currentField = GET_LEFT(current);
            } else {
                currentField = GET_RIGHT(current);
            }
            current = GET_ADDR(currentField);
        }
        return record;
    }


   bool cleanup(val value, seek_record record) {
        node* ancestor = record.ancestor;
        node* successor = record.successor;
        node* parent = record.parent;
        node* leaf = record.leaf;

        node** successor_addr;
        node** sibling_addr;
        node** child_addr;

        if (value < ancestor->value) {
            successor_addr = &(GET_LEFT(ancestor));
        } else {
            successor_addr = &(GET_RIGHT(ancestor));
        }

        if (value < parent->value) {
            child_addr = &(parent->left);
            sibling_addr = &(parent->right);
        } else {
            child_addr = &(parent->right);
            sibling_addr = &(parent->left);
        }

        if (!GET_FLAG(*child_addr))
            sibling_addr = child_addr;
            
        (void) __sync_fetch_and_or(sibling_addr, 1);
        
        bool result = __sync_bool_compare_and_swap(successor_addr, GET_ADDR(successor), UNTAGGED(*sibling_addr));

        if(result){
            delete successor;
        }
        return result;
    }

    // static void free_node(node* node);
};
