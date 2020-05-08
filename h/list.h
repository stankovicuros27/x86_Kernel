#ifndef LIST_H
#define LIST_H

#include "locks.h"
#include "types.h"
#include <iostream.h>

template<class T> class List {

public:
    class Iterator;

    struct Elem {
        T data;
        Elem *prev, *next;
        Elem(T data) : data(data), prev(nullptr), next(nullptr) {}
    };

    Iterator begin() {
        return Iterator(this, head);
    }

    Iterator end() {
        return Iterator(this, nullptr);
    }

    List() : head(nullptr), tail(nullptr), n(0) {}

    virtual ~List() {
        clear();
    }

    bool isEmpty() const {
        return n == 0;
    }

    int size() const {
        return n;
    }

    void clear() {
        Elem* toDelete;
        while (head != nullptr) {
            toDelete = head;
            head = head->next;
            delete toDelete;
        }
        head = tail = nullptr;
        n = 0;
    }

    void remove(Elem *toDelete) {
        if(head == nullptr || toDelete == nullptr) return;
        if(head == toDelete) head = head->next;
        if(tail == toDelete) tail = tail->prev;
        if(toDelete->next != nullptr){
            toDelete->next->prev = toDelete->prev;
        }
        if(toDelete->prev != nullptr){
            toDelete->prev->next = toDelete->next;
        }
        delete toDelete;
        n--;
    }

    void insertBack(T data) {
        Elem* newEl = new Elem(data);
        if (n == 0) {
            head = newEl;
        }
        else {
            newEl->prev = tail;
            tail->next = newEl;
        }
        tail = newEl;
        n++;
    }

    void insertFront(T data) {
        Elem* newEl = new Elem(data);
        if (n == 0) {
            tail = newEl;
        }
        else {
            newEl->next = head;
            head->prev = newEl;
        }
        head = newEl;
        n++;
    }

    T getFront() const {
        return head->data;
    }

    T getBack() const {
        return tail->data;
    }

    void deleteFront() {
        Elem* toDelete = head;
        head = head->next;
        n--;
        if (n == 0) head = tail = nullptr;
        else head->prev = nullptr;
        delete toDelete;
    }

    void deleteBack() {
        Elem* toDelete = tail;
        tail = tail->prev;
        n--;
        if (n == 0) head = tail = nullptr;
        else tail->next = nullptr;
        delete toDelete;
    }

    void printFwd() {
        //aa//
		for (Elem *p = head; p!=nullptr; p=p->next)
			cout << p->data << " ";
		cout << endl;
	}

private: 
    int n;
    Elem* head, * tail;
    friend class Iterator;

public:
    class Iterator {
        public:
            Iterator(List<T>* myList, Elem* current) : myList(myList), current(current){}

            bool operator!=(const Iterator iter) {
                return current != iter.current;
            }

            Iterator& operator++() {
                current = current->next;
                return *this;
            }

            Iterator operator++(int) {
                Iterator old(*this);
                current = current->next;
                return old;
            }

            T& operator*() {
                return current->data;
            }

            void insertBefore(T data) {
			    if (myList->isEmpty() || current->prev == nullptr) myList->insertFront(data); 
			    else if (current == nullptr) myList->insertBack(data); 
			    else { 
                    myList->n++;
				    List<T>::Elem *newelem = new List<T>::Elem(data);
				    newelem->prev = current->prev;
                    newelem->next = current;
				    current->prev->next = newelem;
				    current->prev = newelem;
			    }
		    }

            void remove(){
                myList->remove(current);
                current = nullptr;
            }

        public:
            Elem* current;
            List<T> *myList;
    };
};

#endif