/** -----------------------------------------------------------------------------
  * FILE:    event.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Class-independent event object.
  *          Can register and call multiple functions sequentially.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <vector>
#include <functional>
#include <stdexcept>

namespace pt{

//TODO: removal functions don't yet support 'T&&'
//TODO: indexof 'int' type to 'int32_t'

#define ALLOW_MULTIPLE_INSTANCES 0x1

enum class ExecRule{
    Persistent = 0,
    TriggerOnce = 1
};

/** @class EventTrigger:
 *  @brief Class-independent event object. Has to be wrapped by an Event instance.
 *         The two can register and call multiple functions sequentially in the order of registration.
 */
template<typename... Signature>
class EventTrigger
{
    template<typename...>
    friend class Event; //Event exposes EventTrigger's handling functions that are private

    struct data
    {
        void*                             target;           //used for identification
        void*                             function_ptr;     //used for identification
        std::function<void(Signature...)> function_obj;     //used during calling
        ExecRule                          rule;

        //ctor
        data():target(nullptr),
            function_ptr(nullptr),
            function_obj()
        {}


        //ctor
        data( void* t, void* fptr, std::function<void(Signature...)> fobj,
              ExecRule execrule = ExecRule::Persistent ):
            target(t), function_ptr(fptr), function_obj(fobj), rule(execrule)
        {}


        /**
         * @brief operator ==:
         *   used with object sample during searches
         *      - doesn't check std::function member
         *      - if 'function_ptr' is nullptr, then returns true for every entry with 'target'
         *      - if 'target' is nullptr, then it's a non-class function
         */
        inline bool operator==(const data& other)const
        {
            if(target == other.target){
                if( (other.function_ptr == nullptr) //if sample only contains listener id, then function_ptr doesn't matter
                    ||(function_ptr == other.function_ptr)){
                    return true;
                }
            }
            return false;
        }


        inline bool is_callable() const
        {
            return (function_ptr!=nullptr);
        }


        inline void invalidate()
        {
            target = nullptr;
            function_ptr = nullptr;
            function_obj = nullptr;
        }
    };

    unsigned            mFlags;     //CHECK: may not be needed
    size_t              mSize;
    size_t              mCap;
    size_t              mIndex;
    EventTrigger::data* mFunctions;


    /** @brief: returns the index of the element passed,
     *            or -1 if not contained
     */
    inline int index_of(const EventTrigger::data& d) const
    {
        for(int i=0; i<mIndex; ++i){
            if(mFunctions[i] == d){
                return i;
            }
        }
        return -1;
    }


    inline void defragment_from( EventTrigger::data* from, int const from_cap)
    {
        int i = 0;
        int j = 0;
        while( (i<from_cap) && (j<mCap) ){
            if( from[i].is_callable() ){
                mFunctions[j] = from[i];
                ++j;
            }
            ++i;
        }
        mIndex = j;
    }


    //common mechanics of adding elements
    inline void add_element(EventTrigger::data d)
    {
        if(nullptr == mFunctions){
            reserve(1);
        }

        if( -1 == index_of( d ) ){
            if( mIndex >= mCap ){ //reached end
                if( mSize <= mCap/2 ){ //fragmented enough
                    optimize();
                }else{
                    reserve( 2 * mCap );
                }
            }
            mFunctions[mIndex] = d;
            ++mIndex;
            ++mSize;
        }
        //TODO: add support multiple registrations
    }


    //common mechanics of removing elements
    inline void remove_element(EventTrigger::data d)
    {
        int index = index_of(d);
        if( -1 != index ){
            mFunctions[index].invalidate();
            --mSize;
        }
        //TODO: add support for removing multiple occurences
    }


    //--------------------------------------------------
    //  private functions, that are exposed in Event
    //--------------------------------------------------


    template<typename T>
    inline void addCallback(T* instance, void (T::*func)(Signature...), ExecRule execrule = ExecRule::Persistent )
    {
        if(nullptr == instance){
            throw std::invalid_argument("attempted to register nullptr as listener");
        }else if(nullptr == func){
            throw std::invalid_argument("attempted to register nullptr as function");
        }

        auto lambda = [=](Signature... args) {
            (instance->*func)(args...);
        };

        add_element( EventTrigger::data(reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), lambda, execrule) );
    }


    template<typename T>
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const, ExecRule execrule = ExecRule::Persistent )
    {
        if(nullptr == instance){
            throw std::invalid_argument("attempted to register nullptr as listener");
        }else if(nullptr == func){
            throw std::invalid_argument("attempted to register nullptr as function");
        }

        auto lambda = [=](Signature... args) {
            (instance->*func)(args...);
        };

        auto instance_id = const_cast<T*>(instance);

        add_element( EventTrigger::data(reinterpret_cast<void*>(instance_id), reinterpret_cast<void*>(func), lambda, execrule) );
    }


    //CHECK: with 'T&&' version introduced, is this needed?
    inline void addCallback( void (*func)(Signature...), ExecRule execrule = ExecRule::Persistent )
    {
        if( nullptr == func ){
            throw std::invalid_argument("attempted to register nullptr as function");
        }
        add_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), func, execrule) );
    }


    template<typename T>
    inline void addCallback( T&& func, ExecRule execrule = ExecRule::Persistent )
    {
        add_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(&func), func, execrule) );
    }


    template<typename T>
    inline void removeCallback(T* instance, void (T::*func)(Signature...) )
    {
        if( nullptr == instance ){
            throw std::invalid_argument("attempted to unregister nullptr as listener");
        }else if( nullptr == func ){
            throw std::invalid_argument("attempted to unregister nullptr as function");
        }
        remove_element( EventTrigger::data(reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), nullptr) );
    }


    inline void removeCallback(void (*func)(Signature...) )
    {
        if( nullptr == func ){
            throw std::invalid_argument("attempted to unregister nullptr as function");
        }
        remove_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), nullptr) );
    }


    inline void removeObject(const void* const object)
    {
        if( nullptr == object ){
            throw std::invalid_argument("attempted to unregister nullptr as listener");
        }

        void* object_id = const_cast<void*>(object);

        EventTrigger::data d( reinterpret_cast<void*>(object_id), nullptr, nullptr);

        //loop until cannot find any more entries with 'target'
        int index = 0;
        while( -1 < index ){
            index = index_of(d);
            if( -1 != index ){
                mFunctions[index].invalidate();
                --mSize;
            }
        }
    }


    inline void clear()
    {
        for(size_t i=0; i<mIndex; ++i){
            mFunctions[i].invalidate();
        }
        mIndex = 0;
        mSize = 0;
    }


    inline void reserve(const size_t new_size)
    {
        if(mCap < new_size){
            EventTrigger::data* old = mFunctions;
            mFunctions = new EventTrigger::data[new_size];
            if(old){
                defragment_from(old, mIndex);
            }
            mCap = new_size;
            delete[] old;
        }
    }


    inline void optimize()
    {
        if(mSize < mIndex ){
            defragment_from(mFunctions, mIndex);
        }
    }


    inline void shrink_to_fit()
    {
        if(0 == mSize){
            delete[] mFunctions;
            mCap = 0;
            mIndex = 0;
            mFunctions = nullptr;
        }else{
            if(mSize < mIndex){
                EventTrigger::data* old = mFunctions;
                mFunctions= new EventTrigger::data[mSize];
                defragment_from(old, mIndex);
                mCap = mSize;
                delete[] old;
            }
        }
    }


    //--------------------------------------------------

public:
    EventTrigger(): mFlags(0), mSize(0),
                 mCap(0), mIndex(0),
                 mFunctions(nullptr)
    {}


    EventTrigger(const EventTrigger& other): mFlags(other.mFlags),
                                             mSize(other.mSize),
                                             mCap(other.mCap),
                                             mIndex(other.mIndex)
    {
        mFunctions = new EventTrigger::data[mCap];
        for(int i=0; i<mCap; ++i){
            mFunctions[i] = other.mFunctions[i];
        }
    }


    EventTrigger(EventTrigger&& source): mFlags(source.mFlags),
                                         mSize(source.mSize),
                                         mCap(source.mCap),
                                         mIndex(source.mIndex)
    {
        delete[] mFunctions;
        mFunctions = source.mFunctions;
        source.mFunctions = nullptr;
    }


    virtual ~EventTrigger()
    {
        delete[] mFunctions;
    }


    EventTrigger& operator=(const EventTrigger& other)
    {
        //TODO: new can throw exception here, data will be freed, but not allocated
        delete[] mFunctions;
        mFlags = other.mFlags;
        mSize = other.mSize;
        mCap = other.mCap;
        mIndex = other.mIndex;
        mFunctions = new EventTrigger::data[mCap];
        for(int i=0; i<mCap; ++i){
            mFunctions[i] = other.mFunctions[i];
        }
    }


    EventTrigger& operator=(EventTrigger&& source)
    {
        delete[] mFunctions;
        mFlags = source.mFlags;
        mSize = source.mSize;
        mCap = source.mCap;
        mIndex = source.mIndex;
        mFunctions = source.mFunctions;
        source.mFunctions = nullptr;
    }


    bool operator==(const EventTrigger& other)const = delete;


    /**
     * @brief Fires the event, sequentially executing all
     *          registered functions in the order of registration.
     */
    inline void operator()(Signature...args)
    {
        if(mFunctions){
            for( size_t i=0; i<mIndex; ++i ){
                if( mFunctions[i].is_callable() ){
                    mFunctions[i].function_obj(args...);
                    if( ExecRule::TriggerOnce == mFunctions[i].rule ){
                        mFunctions[i].invalidate();
                        --mSize;
                    }
                }
            }
        }
    }

}; //end of class 'EventTrigger'



/** @class Event:
 *  @brief Wrapper for EventTrigger that hides its 'operator()' and exposes its private handling functions.
 *          Its purpose is to prevent external code to call the EventTrigger with an exposed 'operator()'.
 */
template<typename... Signature>
class Event
{
    EventTrigger<Signature...>& ev_trigger;
public:
    Event(EventTrigger<Signature...>& eventtrigger):
        ev_trigger(eventtrigger){
    }
    Event(const Event& other)                   = delete;  //Note: Event-EventTrigger pairing has to be manually restored by owner object during copy!
    Event(Event&& source)                       = delete;
    virtual ~Event(){}
    Event& operator=(const Event& other)        = delete;
    Event& operator=(Event&& source)            = delete;

    bool operator==(const Event& other) const   = delete;


    /**
     * @brief Registers the constant class member function received in the parameters.
     * @param instance: Pointer to the target object.
     * @param func: Pointer to the member function to call on the target object.
     * @param execrule: Controls whether the function should only be executed once.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const, ExecRule execrule = ExecRule::Persistent )
    {
        ev_trigger.addCallback(instance, func, execrule);
    }


    /**
     * @brief Registers the class member function received in the parameters.
     * @param instance: Pointer to the target object.
     * @param func: Pointer to the member function to call on the target object.
     * @param execrule: Controls whether the function should only be executed once.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void addCallback(T* instance, void (T::*func)(Signature...), ExecRule execrule = ExecRule::Persistent )
    {
        ev_trigger.addCallback(instance, func, execrule);
    }


    /**
     * @brief Registers the basic function received in the parameters.
     * @param func: Function to call.
     * @param execrule: Controls whether the function should only be executed once.
     * @throws std::invalid_argument
     */
    inline void addCallback( void (*func)(Signature...), ExecRule execrule = ExecRule::Persistent )
    {
        ev_trigger.addCallback(func, execrule);
    }


    /**
     * @brief Registers the function/functor received in the parameters.
     * @param func: Function to call.
     * @param execrule: Controls whether the function/functor should only be executed once.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void addCallback( T&& func, ExecRule execrule = ExecRule::Persistent )
    {
        ev_trigger.addCallback(func, execrule);
    }


    /**
     * @brief Removes the class member function defined in the parameters.
     * @param instance: Reference to the target object.
     * @param func: Class member function to call on the target object.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void removeCallback(T* instance, void (T::*func)(Signature...) )
    {
        ev_trigger.removeCallback(instance, func);
    }


    /**
     * @brief Removes the standard function defined in the parameters.
     * @param func: Function to remove from the array.
     * @throws std::invalid_argument
     */
    inline void removeCallback(void (*func)(Signature...) )
    {
        ev_trigger.removeCallback(func);
    }


    /**
     * @brief Removes all function registrations regarding the object received as parameter.
     *   Note: this will remove any parent's member functions as well,
     *   which the caller may not know about.
     * @param object: Listener, whose every registered funcion is to be removed.
     * @throws std::invalid_argument
     */
    inline void removeObject(const void* const object)
    {
        ev_trigger.removeObject(object);
    }


    /**
     * @brief Removes all function registrations regarding the object received as parameter.
     *   Note: this will remove any parent's member functions as well,
     *   which the caller may not know about.
     * @param object: Listener, whose every registered funcion is to be removed.
     * @throws std::invalid_argument
     */
    inline void removeObject(void* object)
    {
        ev_trigger.removeObject(object);
    }


    /**
     * @brief Removes all registered elements. Allocated memory stays.
     */
    inline void clear(){
        ev_trigger.clear();
    }


    /**
     * @brief Ensures, that 'size' amount of entries are allocated in memory for the queue.
     * @param new_size: The amount of entries we want to be allocated.
     */
    inline void reserve(const size_t new_size)
    {
        ev_trigger.reserve(new_size);
    }


    /**
     * @brief Rearranges storage by clumping together still active entries in memory.
     */
    inline void optimize()
    {
        ev_trigger.optimize();
    }


    /**
     * @brief Dumps unnecessary allocated memory.
     */
    inline void shrink_to_fit()
    {
        ev_trigger.shrink_to_fit();
    }
}; //end of class 'Event'

} //end of namespace 'pt'
