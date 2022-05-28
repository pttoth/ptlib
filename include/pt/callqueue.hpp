/** -----------------------------------------------------------------------------
  * FILE:    callqueue.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE:
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <vector>
#include <functional>
#include <stdexcept>

#include <assert.h>

//#include "pt/logging.h"
//#include "pt/utility.hpp"
#include "logging.h"
#include "utility.hpp"

namespace pt{

/** @class EventTrigger:
 *  @brief Can register and call multiple functions sequentally in the order of registration.
 *     'Tid' has to implement 'CallQueue::Identifier' interface
 */
template<typename Tid, typename... Signature>
class CallQueue
{
public:
    struct Identifier
    {
    public:
        virtual bool operator==(const Tid& rhs) = 0;

        virtual void invalidate() = 0;
        virtual bool isValid() const = 0;
    };

    class DefaultIdentifier: public Identifier<>
    {
    private:
        void* target = nullptr;
        void* function_ptr = nullptr;
    public:
        DefaultIdentifier()
        {}

        DefaultIdentifier(void* t, void* fptr):
            target(t), function_ptr(fptr)
        {}

        virtual bool operator==(const DefaultIdentifier& rhs) override{
            return ()
        }

        virtual void invalidate() override{

        }

        virtual bool isValid() const override{

        }

    };

    using FuncType = std::function< void(Signature...) >;

    CallQueue(){
        //TODO: implement
    }
    virtual ~CallQueue(){}

    //copy (forbidden)
    //  Element removal is responsibility of the registrar.
    //  If copy was allowed, the registrar can/will miss out
    //     on the removal-responsibilities in the copied instance.
    CallQueue(const CallQueue& rhs) = delete;
    CallQueue& operator=(const CallQueue& rhs) = delete;

    //copy is forbidden -> this has no value
    bool operator==(const CallQueue& rhs) const = delete;

    //move
    CallQueue(CallQueue&& rhs);
    CallQueue& operator=(CallQueue&& rhs);





private:

    struct DataStorageType{
        Tid         id;
        FuncType    func;

        DataStorageType()
        {}

        bool operator==(const DataStorageType& rhs) const{
            return id == rhs.id;
        }
        bool operator==(const Tid& rhs) const{
            return id == rhs;
        }
    };
    FuncType function_obj;


    std::vector< DataStorageType > mData;


    unsigned            mFlags;
    size_t              mSize;
    size_t              mIndex;


    inline void DefragmentFrom( DataStorageType* from, size_t count)
    {
        size_t cap = mData.capacity();
        size_t i = 0;
        size_t j = 0;

        //if storage is less than the amount of elements to copy
        //  assert, otherwise skip extra elements
        assert(count <= cap);

        Identifier& element = from[i].first;

        while( (i<count) && (j<cap) ){
            if( element.isValid() ){
                mData[j] = from[i];
                ++j;
            }
            ++i;
        }
        mIndex = j;
    }


    /**
     * @brief AddElement:
     *   Adds the element, even if one with 'id' is already contained.
     */
    inline void AddElement(DataStorageType& elem)
    {
        //ensure there's empty space at the back
        if( mCap <= mIndex ){
            if( mSize <= mCap/2 ){  //fragmented enough, skip alloc
                optimize();
            }else{                  //otherwise, allocate new space
                size_t cap = mData.capacity();
                reserve( cap * 2 );
            }
        }
        mData[mIndex] = elem;
        ++mIndex;
        ++mSize;
    }


    /**
     * @brief RemoveElement:
     *   Removes the oldest entry with the 'id'.
     */
    inline void RemoveElement(DataStorageType& elem)
    {
        int idx = pt::IndexOfInVector(mData, elem);
        if( -1 < idx ){
            mData[idx].id.invalidate();
            --mSize;
        }
    }


    /**
     * @brief RemoveAllElementsWithID:
     *   Removes all entries with 'id'.
     */
    inline void RemoveAllElementsWithID(DataStorageType& elem)
    {
        int idx = 0;
        while ( -1 < (idx = pt::IndexOfInVector(mData, elem) ) ){
            mData[idx].id.invalidate();
            --mSize;
        }
    }


    template<typename T>
    inline void AddCallback(Tid& id, T* instance, void (T::*func)(Signature...) )
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

    template<typename T>
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const)      //FUNC_PARAMS
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

    inline void addCallback( void (*func)(Signature...) )          //FUNC_PARAMS
    {
        if( nullptr == func ){
            throw std::invalid_argument("attempted to register nullptr as function");
        }
        add_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), func) );
    }



















public:


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

    //--------------------------------------------------
    //  private functions, that are exposed in Event
    //--------------------------------------------------

    template<typename T>
    inline void addCallback(T* instance, void (T::*func)(Signature...) )      //FUNC_PARAMS
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

    template<typename T>
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const)      //FUNC_PARAMS
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

    inline void addCallback( void (*func)(Signature...) )          //FUNC_PARAMS
    {
        if( nullptr == func ){
            throw std::invalid_argument("attempted to register nullptr as function");
        }
        add_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), func) );
    }

    template<typename T>
    inline void removeCallback(T* instance, void (T::*func)(Signature...) )      //FUNC_PARAMS
    {
        if( nullptr == instance ){
            throw std::invalid_argument("attempted to unregister nullptr as listener");
        }else if( nullptr == func ){
            throw std::invalid_argument("attempted to unregister nullptr as function");
        }
        remove_element( EventTrigger::data(reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), nullptr) );
    }

    inline void removeCallback(void (*func)(Signature...) )                //FUNC_PARAMS
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
 *  @brief Wrapper for EventTrigger that hides its 'operator()' and exposes its private handling functions.
 *          Its purpose is to prevent external code to call the EventTrigger with an exposed 'operator()'.
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

    /**
     * @brief Registers the class member function received in the parameters.
     * @param instance: Pointer to the target object.
     * @param func: Pointer to the member function to call on the target object.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const )
    {
        ev_base.addCallback(instance, func);
    }

    /**
     * @brief Registers the constant class member function received in the parameters.
     * @param instance: Pointer to the target object.
     * @param func: Pointer to the member function to call on the target object.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void addCallback(T* instance, void (T::*func)(Signature...) )
    {
        ev_base.addCallback(instance, func);
    }

    /**
     * @brief Registers the standard function received in the parameters.
     * @param func: Function to call on the target object.
     * @throws std::invalid_argument
     */
    inline void addCallback( void (*func)(Signature...) )
    {
        ev_base.addCallback(func);
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
        ev_base.removeCallback(instance, func);
    }

    /**
     * @brief Removes the standard function defined in the parameters.
     * @param func: Function to remove from the array.
     * @throws std::invalid_argument
     */
    inline void removeCallback(void (*func)(Signature...) )
    {
        ev_base.removeCallback(func);
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
        ev_base.removeObject(object);
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
        ev_base.removeObject(object);
    }

    /**
     * @brief Ensures, that 'size' amount of entries are allocated in memory for the queue.
     * @param new_size: The amount of entries we want to be allocated.
     */
    inline void reserve(const size_t new_size)
    {
        ev_base.reserve(new_size);
    }

    /**
     * @brief Rearranges storage by clumping together still active entries in memory.
     */
    inline void optimize()
    {
        ev_base.optimize();
    }

    /**
     * @brief Dumps unnecessary allocated memory.
     */
    inline void shrink_to_fit()
    {
        ev_base.shrink_to_fit();
    }
}; //end of 'Event'

} //end of namespace PT
