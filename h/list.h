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

    List(const List<T>& list) : head(nullptr), tail(nullptr), n(0) {
        copy(list);
    }

    List& operator=(const List<T>& list) {
        if (this != &list) {
            clear();
            copy(list);
        }
        return *this;
    }

    bool isEmpty() const {
        return n == 0;
    }

    int size() const {
        return n;
    }

    void copy(const List<T>& list) {
        Elem* iter = list.head;
        while (iter != nullptr) {
            insertBack(iter->data);
            iter = iter->next;
        }
        n = list.n;
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

    virtual ~List() {
        clear();
    }

private: 
    int n;
    Elem* head, * tail;
    friend class Iterator;

public:
    class Iterator {
        public:
            Iterator(List<T>* myList, Elem* current) : myList(myList), current(current){}
            Iterator(Elem* current) : current(current), myList(nullptr) {}
            Iterator(List<T>* myList) : current(nullptr), myList(myList) {}

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

            void remove(){
                if (current == nullptr || myList == nullptr) return;
                List<T>::Elem *prev = current->prev, *next = current->next, *toDelete = current;

			    if (prev!=nullptr) prev->next = next;
			    else myList->head = next;

			    if (next!=nullptr) next->prev = prev;
			    else myList->tail = prev;

			    current = nullptr;

			    delete toDelete;
                
			    myList->n--;
            }

            void insertBefore(T data) {
			    if (myList->size() == 0) myList->insertFront(data); // empty list
			    else if (current == nullptr) myList->insertBack(data); // end of list
			    else if (current->prev == nullptr) myList->insertFront(data); // before first
			    else { //inside
				    List<T>::Elem *newelem = new List<T>::Elem(data);
				    newelem->next = current;
				    newelem->prev = current->prev;
				    current->prev->next = newelem;
				    current->prev = newelem;
				    myList->n++;
			}
		}

        private:
            Elem* current;
            List<T> *myList;
    };
};

#endif