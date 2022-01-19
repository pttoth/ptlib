#pragma once

#include <vector>

namespace pt{

template<typename T>
class array
{
    std::vector<T> m_data;

public:
    //-----
    //auto generated ctors/operators
    array() = default;
    array(const array<T>& other) = default;
    array(array<T>&& source) = default;
    ~array(){}

    array& operator=(const array<T>& other) = default;
    array& operator=(array<T>&& source) = default;

    bool operator==(const array<T>& rhs) const = delete;
    //-----

    array(size_t size){
        m_data(size);
    }

    array& operator=(std::initializer_list<T> ilist){
        m_data = ilist;
    }

    //size management
    void reserve(size_t new_cap){
        m_data.reserve(new_cap);
    }

    void resize(size_t new_size){
        m_data.resize(new_size);
    }

    void resize(size_t count, const T& value){
        m_data.resize(count, value);
    }

    void shrink_to_fit(){
        m_data.shrink_to_fit();
    }

    bool is_empty() const{
        return m_data.empty();
    }

    size_t size() const{
        return m_data.size();
    }

    size_t max_size() const{
        return m_data.max_size();
    }

    size_t capacity() const{
        return m_data.capacity();
    }

    //data manipulation
    void assign(size_t count, const T& value){
        m_data.assign(count, value);
    }

    void assign(std::initializer_list<T> ilist){
        m_data.assign(ilist);
    }

    void clear(){
        m_data.clear();
    }

    void push_back(T& elem){
        m_data.push_back(elem);
    }

    void push_back(T&& elem){
        m_data.push_back(elem);
    }

    void pop_back(){
        m_data.pop_back();
    }

    void swap(array& other){
        m_data.swap(other);
    }

    T& at( size_t pos ){
        return m_data.at(pos);
    }

    const T& at( size_t pos ) const{
        return m_data.at(pos);
    }

    T& operator[](size_t pos){
        return m_data[pos];
    }

    const T& operator[](size_t pos) const{
        return m_data[pos];
    }

    T* data(){
        return m_data.data();
    }

    const T* data() const{
        return m_data.data();
    }




/*
    T& front();
    const T& front() const;

    T& back();
    const T& back() const;
*/

    /*
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(iterator first, iterator last);
    iterator erase(const_iterator first, const_iterator last);
    */

    /*
    template< class... Args >
    iterator emplace( const_iterator pos, Args&&... args );

    template< class... Args >
    void emplace_back( Args&&... args );
    */

    /*
    iterator insert( iterator pos, const T& value );
    iterator insert( const_iterator pos, T&& value );
    void insert( iterator pos, size_type count, const T& value );

    template< class InputIt >
    void insert( iterator pos, InputIt first, InputIt last );

    iterator insert( const_iterator pos, std::initializer_list<T> ilist );
    */



    //iterators...
    /*
    begin
    cbegin
    end
    cend
    */







};


template<typename T>
bool operator==(const array<T>& lhs,
                const array<T>& rhs ){
    if(lhs.size != rhs.size())
        return false;

    for(size_t i = 0; i<lhs.size(); ++i){
        if(lhs[i] != rhs[i])
            return false;
    }
    return true;
}

template<typename T>
bool operator!=(const array<T>& lhs,
                const array<T>& rhs ){
    return !(operator==<T>(lhs, rhs));
}


/*
template< class T>
bool operator<(const array<T>& lhs,
               const array<T>& rhs );

template< class T>
bool operator<=(const array<T>& lhs,
                const array<T>& rhs );

template< class T>
bool operator>(const array<T>& lhs,
               const array<T>& rhs );

template< class T>
bool operator>=(const array<T>& lhs,
                const array<T>& rhs );
*/




} //end of namespace 'pt'








