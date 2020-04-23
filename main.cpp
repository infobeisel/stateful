#include <iostream>
#include "Stateful.h"



class StatefulMethods {
public:
    virtual void saySth() = 0; 
};

class User : public Stateful<StatefulMethods,User> {
    friend class Verbose;
    friend class Idle;
public:
    User();
    void saySth();
private:
    int someData;
};

class Verbose : public State<StatefulMethods,User> {
public:
    Verbose(User & backRef) :
    State(backRef) {std::cout << "construct Verbose" << std::endl;}
    ~Verbose() {std::cout << "destruct Verbose" << std::endl;}
     void saySth() override {
        std::cout << "very much blah" << std::endl;
    }
protected:
    STATEPTR(StatefulMethods,User) nextState() override {
        RETURNSELF
    }
    void entry() override {
        std::cout << "init Verbose" << std::endl;
        back_ref.someData++;
    }
    void exit() override {
        std::cout << "cleanup Verbose" << std::to_string(back_ref.someData) << std::endl;
        
    }
};

class Idle : public State<StatefulMethods,User> {
public:
    Idle(User & backRef) :
    State(backRef) , saidCoucou(false) {std::cout << "construct Idle" << std::endl;}
    ~Idle() {std::cout << "construct Idle" << std::endl;}
    void saySth() override {
        std::cout << "cou couu" << std::endl;
        saidCoucou = true;
    }
protected:
    STATEPTR(StatefulMethods,User) nextState() override {
        if(saidCoucou) {
            return std::make_unique<Verbose>(back_ref);
        }
        RETURNSELF
    }   
    void entry() override {
        std::cout << "init Idle" << std::endl;
        back_ref.someData = 1;
    }
    void exit() override {
        std::cout << "cleanup Idle" << std::endl;
        back_ref.someData++;
    }
private:
    bool saidCoucou;
};

User::User() : Stateful(std::make_unique<Idle>(*this)) {}

void User::saySth() {
    state->saySth();
    updateState();
}

int main() {

    User usr;
    usr.saySth();
    usr.saySth();
    usr.saySth();
    
}