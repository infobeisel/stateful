#include <memory>

/*
to make your class X stateful:

1. first create an abstract class Y containing all methods that each of your states 
should implement.

class Y {
public:
    virtual void mymethod() = 0;
};

2. then create your first state S your stateful class X should start in, make it inherit from
State<Y,X>, and make sure to call its constructor. in your state you can always access
your main class object X by 
back_ref.data 
you could declare your State classes as friendship classes in your main class X to be able
to access private members of your main class

class S : public State<Y,X> {
    S(Stateful<Y,X> & ref) : State(ref) {...}
};

3. Finally inherit your class X from Stateful<Y,X> , and make sure that you call its constructor,
and give it an instance of your start State S, as well as references to this:  
class X : public Stateful<Y,X> {
    X() :  Stateful(std::make_shared<Idle>(*this), *this) 
};

4. now you can call methods from Y on the inherited member state 
in any method in your class X (dont forget to call updateState() afterwards), e.g.

void X::foo() {
    state.mymethod();
}

5. to add more states P and Q , create classes similar to S (see 2.), then override the method
nextState() in each of your states, where you can return another state, or yourself if you dont want
to change the current state:

STATEPTR(Y,X) nextState() override {
    if(...)
        return std::make_shared<P>(back_ref);
    else if(...)
        return std::make_shared<Q>(back_ref);
    else
        return back_ref.state;
}  

*/
template<typename Methods,typename User>
class Stateful ;

template<typename Methods, typename User>
class State : public Methods {
    friend class Stateful<Methods,User>;
protected:
    Stateful<Methods,User> & back_ref;
    virtual std::shared_ptr<State<Methods,User>> nextState() { return back_ref.state;}
    virtual void entry() {}
    virtual void exit() {}
public:
    State(Stateful<Methods,User> & backRef) : back_ref(backRef) {}
};

template<typename Methods,typename User>
class Stateful {
    friend class State<Methods,User>;
public: 
    Stateful(std::shared_ptr<State<Methods,User>> startState, User & selfRef) : state(startState), data(selfRef) 
    {
        state->entry();
    }
    ~Stateful()
    {
        state->exit();
    }
    void updateState() {
        auto next = state->nextState();
        if(next != state) {
            state->exit();
            state = next;
            next->entry();
        }
    }
    std::shared_ptr<State<Methods,User>> state;
    User & data;
};

#define STATEPTR(T1,T2) std::shared_ptr<State<T1,T2>>

