/** -----------------------------------------------------------------------------
  * FILE:    event.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Class-independent event object.
  *          Can register and call multiple functions sequentally.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <vector>
#include <functional>
#include <stdexcept>

namespace pt{

#define ALLOW_MULTIPLE_INSTANCES 0x1

template<typename... Signature>
class EventTrigger
{
    struct data
    {
        void*                             target;           //used for identification
        void*                             function_ptr;     //used for identification
        std::function<void(Signature...)> function_obj;     //used during calling       //FUNC_PARAMS

        //ctor
        data():target(nullptr),
            function_ptr(nullptr),
            function_obj()
        {}

        //ctor
        data(void* t,
             void* fptr,
             std::function<void(Signature...)> fobj): target(t),    //FUNC_PARAMS
                                                      function_ptr(fptr),
                                                      function_obj(fobj)
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
    size_t              mIndex;     //CHECK: queue may be fragmented, so that index != size  (fix occurences!!!!)
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
            ++mIndex; //CHECK: optimize and reserve fixed index???
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
     * @brief Registers the class member function received in the parameters.
     * @param instance: pointer to the target object
     * @param func: point to the member function to call on the target object
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void add(T* instance, void (T::*func)(Signature...) )      //FUNC_PARAMS
    {
        if(nullptr == instance){
            throw std::invalid_argument("attempted to register nullptr as listener");
        }else if(nullptr == func){
            throw std::invalid_argument("attempted to register nullptr as function");
        }

        auto lambda = [=](Signature... args) {
            (instance->*func)(args...);
        };

        add_element( EventTrigger::data(reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), lambda) ); //FUNC_PARAMS
    }

    /**
     * @brief Registers the (!)constant(!) class member function received in the parameters.
     * @param instance: pointer to the target object
     * @param func: point to the member function to call on the target object
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void add(const T* const instance, void (T::*func)(Signature...) const)      //FUNC_PARAMS
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

        add_element( EventTrigger::data(reinterpret_cast<void*>(instance_id), reinterpret_cast<void*>(func), lambda) ); //FUNC_PARAMS
    }

    /**
     * @brief Registers the global function received in the parameters.
     * @param func: function to call on the target object
     * @throws std::invalid_argument
     */
    inline void add( void (*func)(Signature...) )          //FUNC_PARAMS
    {
        if( nullptr == func ){
            throw std::invalid_argument("attempted to register nullptr as function");
        }
        add_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), func) );
    }

    /**
     * @brief Removes the member function defined in the parameters
     * @param instance: reference to the target object
     * @param func: member function to call on the target object
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void remove(T* instance, void (T::*func)(Signature...) )      //FUNC_PARAMS
    {
        if( nullptr == instance ){
            throw std::invalid_argument("attempted to unregister nullptr as listener");
        }else if( nullptr == func ){
            throw std::invalid_argument("attempted to unregister nullptr as function");
        }
        remove_element( EventTrigger::data(reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), nullptr) );
    }

    /**
     * @brief Removes the global function defined in the parameters
     * @param func: function to remove from the array
     * @throws std::invalid_argument
     */
    inline void remove(void (*func)(Signature...) )                //FUNC_PARAMS
    {
        if( nullptr == func ){
            throw std::invalid_argument("attempted to unregister nullptr as function");
        }
        remove_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), nullptr) );
    }

    /**
     * @brief Removes all function registrations regarding the object received as parameter
     *   Note: this will remove any parent's member functions as well,
     *   which the caller may not know about
     * @param object: listener, whose every registered funcion should be removed
     * @throws std::invalid_argument
     */
    inline void remove_object(void* const object)
    {
        if( nullptr == object ){
            throw std::invalid_argument("attempted to unregister nullptr as listener");
        }

        EventTrigger::data d( reinterpret_cast<void*>(object), nullptr, nullptr);

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

    /**
     * @brief ensures, that 'size' amount of entries are allocated in memory for the queue
     * @param new_size: the amount of entries we want to be allocated
     */
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

    /**
     * @brief Rearranges storage by clumping together still active entries in memory
     */
    inline void optimize()
    {
        //TODO: don't free up memory if empty, this should only be defragmentation
        if(0 == mSize){
            delete[] mFunctions;
            mCap = 0;
            mIndex = 0;
            mFunctions = nullptr;
        }else if(mSize < mIndex ){
            defragment_from(mFunctions, mIndex);
        }
    }

    /**
     * @brief Dumps unnecessary allocated memory
     */
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

    /**
     * @brief Fires the event, sequentally executing all
     *          registered functions in the order of registration.
     */
    inline void operator()(Signature...args)        //FUNC_PARAMS
    {
        if(mFunctions){
            for( size_t i=0; i<mIndex; ++i ){
                if( mFunctions[i].is_callable() ){
                    mFunctions[i].function_obj(args...);    //FUNC_PARAMS
                }
            }
        }
    }

}; //end of 'EventBase'



/** @class Event:
 *  @brief Wrapper class that hides the 'operator()' of the actual event object
 *          Its purpose is to prevent external code to trigger the event with the exposed operator().
 */
template<typename... Signature>
class Event
{
    EventTrigger<Signature...>& ev_base;
public:
    Event(EventTrigger<Signature...>& eventbase):
        ev_base(eventbase){
    }
    Event(const Event& other)                   = delete;  //Note: Event-EventBase pairing has to be manually restored by owner object during copy!
    Event(Event&& source)                       = delete;
    virtual ~Event(){}
    Event& operator=(const Event& other)        = delete;
    Event& operator=(Event&& source)            = delete;

    bool operator==(const Event& other) const   = delete;

    template<typename T>
    inline void add(const T* const instance, void (T::*func)(Signature...) const )
    {
        ev_base.add(instance, func);
    }

    template<typename T>
    inline void add(T* instance, void (T::*func)(Signature...) )
    {
        ev_base.add(instance, func);
    }

    inline void add( void (*func)(Signature...) )
    {
        ev_base.add(func);
    }

    template<typename T>
    inline void remove(T* instance, void (T::*func)(Signature...) )
    {
        ev_base.remove(instance, func);
    }

    inline void remove(void (*func)(Signature...) )
    {
        ev_base.remove(func);
    }

    inline void remove_object(void* const object)
    {
        ev_base.remove_object(object);
    }

    inline void reserve(const size_t new_size)
    {
        ev_base.reserve(new_size);
    }

    inline void optimize()
    {
        ev_base.optimize();
    }

    inline void shrink_to_fit()
    {
        ev_base.shrink_to_fit();
    }
}; //end of 'Event'



} //end of namespace PT
