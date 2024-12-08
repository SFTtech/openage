// Copyright 2017-2024 the openage authors. See copying.md for legal info.


#pragma once

#include <array>

#include "curve/keyframe_container.h"
#include "curve/base_curve.h"


//remember to update docs
namespace openage {
namespace curve {

    template <typename T, size_t s>
    class Array
    {
    public:
        using this_type = Array<T, s>;
        using container_t = typename std::array<KeyframeContainer<T>, s>;

        // prevent accidental copy of queue
        //Array(const Array &) = delete;


        T get(const time::time_t& t, const size_t index) const;
        //T get(const time::time_t& t, const size_t index, const size_t hint) const;

        
 
        std::array<T, s> get(const time::time_t& t) const;
        //std::array<Keyframe<T>, size> get(const time::time_t& t, const size_t hint) const;

        
        size_t size() const;

        const Keyframe<T>& frame(const time::time_t& t, const size_t index) const;
        

        const Keyframe<T>& next_frame(const time::time_t& t, const size_t index) const;

        void set_insert(const time::time_t& t, const size_t index, T value);	
        
        void set_last(const time::time_t& t, const size_t index, T value);	

        void set_replace(const time::time_t& t, const size_t index, T value);	

        void sync(const Array<T, s>& other, const time::time_t& t);

    private:
        container_t container;
    };


    
    template <typename T, size_t size>
    const Keyframe<T>& Array<T, size>::frame(const time::time_t& t, const size_t index) const
    {
        size_t container_index = container[index].last(t);
        return container[index].get(container_index);
    }

    template <typename T, size_t size>
    const Keyframe<T>& Array<T, size>::next_frame(const time::time_t& t, const size_t index) const
    {
        size_t container_index = container[index].last(t);

        return container[index].get(++container_index);
    }
    
    template <typename T, size_t size>
    T Array<T, size>::get(const time::time_t& t, const size_t index) const
    {
        return this->frame().value;
    }

    template <typename T, size_t s>
    std::array<T, s>  Array<T, s>::get(const time::time_t& t) const
    {
        return std::array<T, size>
        {
            []<auto... I>(std::index_sequence<I...>)
            {
                return std::array<T, size> { this->get(t, I)... };
            }(std::make_index_sequence<size>{})
        };
    }

    template <typename T, size_t size>
    size_t Array<T, size>::size() const
    {
        return size;
    }


    template <typename T, size_t size>
    void Array<T, size>::set_insert(const time::time_t& t, const size_t index, T value)
    {
        this->container[index].insert_after(t, value);
    }


    template <typename T, size_t size>
    void Array<T, size>::set_last(const time::time_t& t, const size_t index, T value)
    {
        size_t i = this->container[index].insert_after(t, value);
        this->container[index].erase_after(i);
    }


    template <typename T, size_t size>
    void Array<T, size>::set_replace(const time::time_t& t, const size_t index, T value)
    {
        size_t i = this->container[index].insert_overwrite(t, value);
    }

    template <typename T, size_t s>
    void Array<T, s>::sync(const Array<T, s>& other, const time::time_t& start)
    {  
        for(int i = 0; i < s, i++)
        {
            this->container[i].sync(other, start);
        }
    }



} //curve
} //openage
