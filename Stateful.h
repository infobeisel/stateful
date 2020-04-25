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
back_ref.
tip: declare your State classes as friendship classes in your main class X to be able
to access private members of your main class

class S : public State<Y,X> {
    S(X & ref) : State(ref) {...}
};

3. Finally inherit your class X from Stateful<Y,X> , and make sure that you call its constructor,
and give it an instance of your start State S:  

class X : public Stateful<Y,X> {
    X() :  Stateful(std::make_unique<S>(*this)) 
};

4. now you can call methods from Y on the inherited member state 
in any method in your class X (dont forget to call updateState() afterwards), e.g.

void X::foo() {
    state.mymethod();
    updateState(); //this calls nextState() on the current state, see 5.
}

5. to add more states P and Q , create classes similar to S (see 2.), then override the method
nextState() in each of your states, where you can return another state, or yourself if you dont want
to change the current state (in this case, you must use the macro RETURNSELF). nextState() will always get called when you call updateState() in your main class X

STATEPTR(Y,X) nextState() override {
    if(...)
        return std::make_unique<P>(back_ref);
    else if(...)
        return std::make_unique<Q>(back_ref);
    else
        RETURNSELF
}  

6. please do not use your State constructors and destructors to modify back_ref, instead override and 
use the entry() and exit() methods for this. the reason is that if you switch to a next state, 
the next state's constructor gets called before the old state's destructor.

*/
template<typename Methods,typename User>
class Stateful ;

#define STATEPTR(T1,T2) State<T1,T2> * 
#define RETURNSELF return nullptr;

template<typename Methods, typename User>
class State : public Methods {
    friend class Stateful<Methods,User>;
protected:
    User & back_ref;
    virtual STATEPTR(Methods,User) nextState() { RETURNSELF }
    virtual void entry() {}
    virtual void exit() {}
public:
    State(User & backRef) : back_ref(backRef) {}
    virtual ~State() {}
};

template<typename Methods,typename User>
class Stateful {
    friend class State<Methods,User>;
public: 
    Stateful(State<Methods,User> * startState) : state(startState)
    {
        state->entry();
    }
    ~Stateful()
    {
        state->exit();
        delete state;
    }
    void updateState() {
        auto next = state->nextState();
        //right now, either next is a new object, empty
        if(next) { 
            state->exit();
            delete state;
            state = next;
            state->entry();

        }
    }
    STATEPTR(Methods,User) state;
};


