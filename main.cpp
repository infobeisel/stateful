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
    Verbose(Stateful<StatefulMethods,User> & backRef) :
    State(backRef) {}
     void saySth() override {
        std::cout << "very much blah" << std::endl;
    }
protected:
    STATEPTR(StatefulMethods,User) nextState() override {
        return back_ref.state;
    }
    void entry() override {
        std::cout << "init Verbose" << std::endl;
        back_ref.data.someData++;
    }
    void exit() override {
        std::cout << "cleanup Verbose" << std::to_string(back_ref.data.someData) << std::endl;
        
    }
};

class Idle : public State<StatefulMethods,User> {
public:
    Idle(Stateful<StatefulMethods,User> & backRef) :
    State(backRef) , saidCoucou(false) {}
    void saySth() override {
        std::cout << "cou couu" << std::endl;
        saidCoucou = true;
    }
protected:
    STATEPTR(StatefulMethods,User) nextState() override {
        if(saidCoucou)
            return std::make_shared<Verbose>(back_ref);
        else
            return back_ref.state;
    }   
    void entry() override {
        std::cout << "init Idle" << std::endl;
        back_ref.data.someData = 1;
    }
    void exit() override {
        std::cout << "cleanup Idle" << std::endl;
        back_ref.data.someData++;
    }
private:
    bool saidCoucou;
};

User::User() : Stateful(std::make_shared<Idle>(*this), *this) {}

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