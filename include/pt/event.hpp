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

enum class EventExecRule{
    Persistent = 0,
    TriggerOnce = 1
};

enum class EventRemoveRule{
    One = 0,
    All = 1
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
        //TODO: this way of identification causes "-Wpmf-conversions" warnings
        //  having pointers to member functions violates the cpp standard
        //  GCC allows it, but warns ( what about other compilers? )
        //  see if some trickery can solve this as uint32_t/uint64_t instead of 'void*'
        //  other ways to avoid violating the standard?
        void*                             target        = nullptr;     //used for identification
        void*                             function_ptr  = nullptr;     //used for identification
        std::function<void(Signature...)> function_obj;                //used during calling
        EventExecRule                     rule          = EventExecRule::Persistent;

        //ctor
        data(): function_obj()
        {}


        //ctor
        data( void* t, void* fptr,
              std::function<void(Signature...)> fobj,
              EventExecRule execrule = EventExecRule::Persistent ):
            target(t), function_ptr(fptr), function_obj(fobj), rule(execrule)
        {}


        /**
         * @brief operator ==:
         *   used to compare with a sample object during searches
         *      - doesn't check 'function_obj' (std::function) member
         *      - doesn't check 'rule' member
         *      - if 'function_ptr' is nullptr, then returns true for every entry with 'target'
         *      - if 'target' is nullptr, then it's a non-class function
         */
        inline bool operator==(const data& other)const
        {
            if(target == other.target){
                if( (nullptr == other.function_ptr) //if sample only contains listener id, then function_ptr doesn't matter
                    ||(function_ptr == other.function_ptr))
                {
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

    size_t              mSize = 0;
    size_t              mCap = 0;
    size_t              mIndex = 0;
    EventTrigger::data* mFunctions = nullptr;

    static const std::string& GetErrStrNullFunctionOnAdd(){
        static const std::string str( "Attempted to add nullptr as function to event." );
        return str;
    }

    static const std::string& GetErrStrNullListenerOnAdd(){
        static const std::string str( "Attempted to add nullptr as listener to event." );
        return str;
    }

    static const std::string& GetErrStrNullFunctionOnRemove(){
        static const std::string str( "Attempted to remove nullptr as function from event." );
        return str;
    }

    static const std::string& GetErrStrNullListenerOnRemove(){
        static const std::string str( "Attempted to remove nullptr as listener from event." );
        return str;
    }

    /** @brief: returns the index of the element passed,
     *            or -1 if not contained
     */
    inline int64_t index_of(const EventTrigger::data& d) const
    {
        for( size_t i=0; i<mIndex; ++i ){
            if(mFunctions[i] == d){
                return i;
            }
        }
        return -1;
    }


    inline void defragment_from( EventTrigger::data* from, size_t count)
    {
        size_t i = 0;
        size_t j = 0;
        while( (i<count) && (j<mCap) ){
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
        if( nullptr == mFunctions ){
            reserve(1);
        }

        if( mCap <= mIndex ){ //reached end
            if( mSize <= mCap/2 ){ //fragmented enough
                optimize();
            }else{
                reserve( mCap * 2 );
            }
        }
        mFunctions[mIndex] = d;
        ++mIndex;
        ++mSize;
    }


    //common mechanics of removing 1 element
    inline void remove_element(EventTrigger::data d)
    {
        int32_t index = index_of(d);
        if( -1 < index ){
            mFunctions[index].invalidate();
            --mSize;
        }
    }


    //common mechanics of removing all occurences of element
    inline void remove_element_occurences(EventTrigger::data d)
    {
        for( size_t i=0; i<mIndex; ++i){
            EventTrigger::data& element = mFunctions[i];
            if( element == d ){
                element.invalidate();
                --mSize;
            }
        }
    }


    //--------------------------------------------------
    //  private functions, that are exposed in Event
    //--------------------------------------------------


    template<typename T>
    inline void addCallback(T* instance, void (T::*func)(Signature...), EventExecRule execrule )
    {
        if(nullptr == instance){
            throw std::invalid_argument( GetErrStrNullListenerOnAdd() );
        }else if(nullptr == func){
            throw std::invalid_argument( GetErrStrNullFunctionOnAdd() );
        }

        auto lambda = [=](Signature... args) {
            (instance->*func)(args...);
        };

        add_element( EventTrigger::data(reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), lambda, execrule) );
    }


    template<typename T>
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const, EventExecRule execrule )
    {
        if(nullptr == instance){
            throw std::invalid_argument( GetErrStrNullListenerOnAdd() );
        }else if(nullptr == func){
            throw std::invalid_argument( GetErrStrNullFunctionOnAdd() );
        }

        auto lambda = [=](Signature... args) {
            (instance->*func)(args...);
        };

        auto instance_id = const_cast<T*>(instance);

        add_element( EventTrigger::data(reinterpret_cast<void*>(instance_id), reinterpret_cast<void*>(func), lambda, execrule) );
    }


    //CHECK: with 'T&&' version introduced, is this needed?
/*
    inline void addCallback( void (*func)(Signature...), EventExecRule execrule )
    {
        if( nullptr == func ){
            throw std::invalid_argument( GetErrStrNullFunctionOnAdd() );
        }
        add_element( EventTrigger::data(nullptr, reinterpret_cast<void*>(func), func, execrule) );
    }
*/


    template<typename T>
    inline void addCallback( T&& func, EventExecRule execrule )
    {
        void* function_ptr = reinterpret_cast<void*>(&func);
        if( nullptr == function_ptr ){
            //this is probably impossible, as even explicitly calling with 'nullptr' doesn't result in 'function_ptr' being 'nullptr'
            //  but this MUST NEVER result in 'nullptr' being function ID as it would result in an invalid container state
            //  (insertion would treat is as a valid element, while the 'nullptr' function ID would identify it as an invalidated entry)
            throw std::invalid_argument( GetErrStrNullFunctionOnAdd() );
        }

        add_element( EventTrigger::data(nullptr, function_ptr, func, execrule) );
    }


    template<typename T>
    inline void removeCallback(T* instance, void (T::*func)(Signature...), EventRemoveRule rule )
    {
        if( nullptr == instance ){
            throw std::invalid_argument( GetErrStrNullListenerOnRemove() );
        }else if( nullptr == func ){
            throw std::invalid_argument( GetErrStrNullFunctionOnRemove() );
        }

        if( EventRemoveRule::All == rule ){
            remove_element_occurences( EventTrigger::data( reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), nullptr ) );
        }else{
            remove_element( EventTrigger::data( reinterpret_cast<void*>(instance), reinterpret_cast<void*>(func), nullptr ) );
        }
    }


    //with the 'T&&' version introduced, this is probably not needed
/*
    inline void removeCallback(void (*func)(Signature...), EventRemoveRule rule )
    {
        if( nullptr == func ){
            throw std::invalid_argument( GetErrStrNullFunctionOnRemove() );
        }

        if( EventRemoveRule::All == rule ){
            remove_element_occurences( EventTrigger::data( nullptr, reinterpret_cast<void*>(func), nullptr ) );
        }else{
            remove_element( EventTrigger::data( nullptr, reinterpret_cast<void*>(func), nullptr ) );
        }
    }
*/


    template<typename T>
    inline void removeCallback( T&& func, EventRemoveRule rule )
    {
        void* function_ptr = reinterpret_cast<void*>(&func);
        if( nullptr == function_ptr ){
            //this is probably impossible, as even explicitly calling with 'nullptr' doesn't result in 'function_ptr' being 'nullptr'
            //  but this MUST NEVER result in 'nullptr' being function ID as it would result in an invalid container state
            throw std::invalid_argument( GetErrStrNullFunctionOnRemove() );
        }

        if( EventRemoveRule::All == rule ){
            remove_element_occurences( EventTrigger::data( nullptr, function_ptr, nullptr ) );
        }else{
            remove_element( EventTrigger::data( nullptr, function_ptr, nullptr ) );
        }
    }


    inline void removeObject(const void* const object)
    {
        if( nullptr == object ){
            throw std::invalid_argument( GetErrStrNullListenerOnRemove() );
        }

        void* object_id = const_cast<void*>(object);

        EventTrigger::data d( reinterpret_cast<void*>(object_id), nullptr, nullptr);

        //loop until cannot find any more entries with 'target'
        int32_t index;
        while( -1 < (index = index_of(d)) ){
            mFunctions[index].invalidate();
            --mSize;
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
                defragment_from(old, mIndex); //sets 'mIndex'
            }
            mCap = new_size;
            delete[] old;
        }
    }


    inline void optimize()
    {
        if(mSize < mIndex ){
            defragment_from(mFunctions, mIndex); //sets 'mIndex'
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
                defragment_from(old, mIndex); //sets 'mIndex'
                mCap = mSize;
                delete[] old;
            }
        }
    }


    //--------------------------------------------------

public:
    EventTrigger()
    {}


    EventTrigger(const EventTrigger& other):
        mSize(other.mSize), mCap(other.mCap)
    {
        mFunctions = new EventTrigger::data[mCap];
        defragment_from( other.mFunctions, other.mIndex ); //sets 'mIndex'
    }


    EventTrigger(EventTrigger&& source):
        mSize(source.mSize), mCap(source.mCap), mIndex(source.mIndex)
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
        EventTrigger::data* old = mFunctions;
        mFunctions = new EventTrigger::data[other.mCap];
        mCap = other.mCap;
        mSize = other.mSize;
        defragment_from( other.mFunctions, other.mIndex ); //sets 'mIndex'
        delete[] old;
        return *this;
    }


    EventTrigger& operator=(EventTrigger&& source)
    {
        //don't defragment here, move has to be fast
        delete[] mFunctions;
        mSize = source.mSize;
        mCap = source.mCap;
        mIndex = source.mIndex;
        mFunctions = source.mFunctions;
        source.mFunctions = nullptr;
        return *this;
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
                    if( EventExecRule::TriggerOnce == mFunctions[i].rule ){
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
    inline void addCallback(const T* const instance, void (T::*func)(Signature...) const, EventExecRule execrule = EventExecRule::Persistent )
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
    inline void addCallback(T* instance, void (T::*func)(Signature...), EventExecRule execrule = EventExecRule::Persistent )
    {
        ev_trigger.addCallback(instance, func, execrule);
    }


    /**
     * @brief Registers the basic function received in the parameters.
     * @param func: Function to call.
     * @param execrule: Controls whether the function should only be executed once.
     * @throws std::invalid_argument
     */
/*
    inline void addCallback( void (*func)(Signature...), ExecRule execrule = ExecRule::Persistent )
    {
        ev_trigger.addCallback(func, execrule);
    }
*/


    /**
     * @brief Registers the function/functor received in the parameters.
     *   Note that lambdas registered with 'Persistent' rule can only be removed by wiping the storage with 'clear()'!
     * @param func: Function to call.
     * @param execrule: Controls whether the function/functor should only be executed once.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void addCallback( T&& func, EventExecRule execrule = EventExecRule::Persistent )
    {
        ev_trigger.addCallback(func, execrule);
    }


    /**
     * @brief Removes the class member function defined in the parameters.
     * @param instance: Reference to the target object.
     * @param func: Function to remove.
     * @param mode: Controls whether to remove only one or all registered instances.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void removeCallback(T* instance, void (T::*func)(Signature...), EventRemoveRule mode = EventRemoveRule::One )
    {
        ev_trigger.removeCallback(instance, func, mode);
    }


    /**
     * @brief Removes the standard function defined in the parameters.
     * @param func: Function to remove.
     * @param mode: Controls whether to remove only one or all registered instances.
     * @throws std::invalid_argument
     */
/*
    inline void removeCallback(void (*func)(Signature...), RemoveMode mode = RemoveMode::One )
    {
        ev_trigger.removeCallback(func, mode);
    }
*/


    /**
     * @brief Removes the function/functor defined in the parameters.
     * @param func: Function/functor to remove.
     * @param mode: Controls whether to remove only one or all registered instances.
     * @throws std::invalid_argument
     */
    template<typename T>
    inline void removeCallback( T&& func, EventRemoveRule mode = EventRemoveRule::One )
    {
        ev_trigger.removeCallback(func, mode);
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
