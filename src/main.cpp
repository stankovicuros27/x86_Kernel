#include "system.h"
#include "scheduler.h"
#include "list.h"
#include <iostream.h>

void testList();

int main(int argc, char* argv[]){

    cout << "Starting OS..." << endl;  
    
    System::initializeSystem();

    PCB *nekiKurac = new PCB(4096, 2, testList);
    nekiKurac->state = PCB::READY;
    Scheduler::put(nekiKurac);

    for (int i = 0; i < 30000; i++){
        for (int j = 0; j < 30000; j++){
            for (int k = 0; k < 3; k++);
        }
    }
    
    System::restoreSystem();

    cout << "Terminating OS..." << endl;
    return 0;
}






void testList(){
    List<int> intList;

    for (int i = 0; i < 10; i++){
        intList.insertBack(i);
    }
    for (i = 3; i < 9; i++){
        intList.insertFront(i);
    }
    //3 4 5 6 7 8 0 1 2 .. 8

    cout << intList.isEmpty() << " " << intList.size() << endl;

    cout << intList.getFront() << " " << intList.getBack() << endl;

    intList.printList();

    List<int> intList2 = intList;

    intList2.printList();

    while(!intList2.isEmpty()){
        cout << intList2.getFront();
        intList2.deleteFront();
    }

    cout << endl;

    while(!intList.isEmpty()){
        cout << intList.getBack();
        intList.deleteBack();
    }
}
