#ifndef LIST_H
#define LIST_H

#include "locks.h"
#include "types.h"
#include <iostream.h>

template<class T>
class List{

public:
    struct Elem {
        T data;
        Elem *prev, *next;
        Elem(T data) : data(data), prev(nullptr), next(nullptr) {}
    };

    List() : head(nullptr), tail(nullptr), n(0) {}

    bool isEmpty() const {
        return n==0;
    }

    int size() const {
        return n;
    }

    void clear() {
        Elem *toDelete;
        while(head != nullptr){
            toDelete = head;
            head = head->next;
            //H_LOCKED(
                 delete toDelete;
            //)
        }
        head = tail = nullptr;
        n = 0;
    }

    List(const List<T> &list) : head(nullptr), tail(nullptr), n(0) {
        copy(list);
    }

    List& operator=(const List<T> &list){
        if(this != &list){
            clear();
            copy(list);
        }
        return *this;
    }

    void insertBack(T data){
        //H_LOCKED(
            Elem *newEl = new Elem(data);
        //)
        if(n == 0){
            head = newEl;
        } else {
            newEl->prev = tail;
            tail->next = newEl;
        }
        tail = newEl;
        n++;
    }

    void insertFront(T data){
        //H_LOCKED(
            Elem *newEl = new Elem(data);
        //)
        if(n == 0){
            tail = newEl;
        } else {
            newEl->next = head;
            head->prev = newEl;
        }
        head = newEl;
        n++;
    }

    T getFront() const{
        return head->data;
    }

    T getBack() const{
        return tail->data;
    }

    void deleteFront(){
        Elem *toDelete = head;
        head = head->next;
        n--;
        if(n==0) head = tail = nullptr;
        else head->prev = nullptr;
        //H_LOCKED(
            delete toDelete;
        //)
    }

    void deleteBack(){
        Elem *toDelete = tail;
        tail = tail->prev;
        n--;
        if(n==0) head = tail = nullptr;
        else tail->next = nullptr;
        //H_LOCKED(
            delete toDelete;
        //)
    }

    void printList() const{
        Elem *iter = head;
        while(iter){
            //H_LOCKED(
                cout << iter->data << " ";
            //)
            iter = iter->next;
        }
        //H_LOCKED(
            cout << endl;
        //)
    }

    virtual ~List() {
        clear();
    }
    
public: //bilo je private

    int n;
    Elem *head, *tail;

    void copy(const List<T> &list){
        Elem *iter = list.head;
        while(iter != nullptr){
            insertBack(iter->data);
            iter = iter->next;
        }
        n = list.n;
    }

};

#endif