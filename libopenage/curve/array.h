// Copyright 2017-2024 the openage authors. See copying.md for legal info.


#pragma once

#include <array>

#include "curve/keyframe_container.h"
#include "curve/base_curve.h"


//remember to update docs
namespace openage {
namespace curve {

    template <typename T, size_t Size>
    class Array
    {
    public:
        using this_type = Array<T, Size>;
        using container_t = std::array<KeyframeContainer<T>, Size>;


        //Array() = default;

        //prevent accidental copy of queue
        //Array(const Array &) = delete;


        T get(const time::time_t& t, const size_t index) const;
        //T get(const time::time_t& t, const size_t index, const size_t hint) const;

        
 
        std::array<T, Size> get(const time::time_t& t) const;
        //std::array<Keyframe<T>, size> get(const time::time_t& t, const size_t hint) const;

        
        size_t size() const;

        const Keyframe<T>& frame(const time::time_t& t, const size_t index) const;
        

        const Keyframe<T>& next_frame(const time::time_t& t, const size_t index) const;

        void set_insert(const time::time_t& t, const size_t index, T value);	
        
        void set_last(const time::time_t& t, const size_t index, T value);	

        void set_replace(const time::time_t& t, const size_t index, T value);	

        void sync(const Array<T, Size>& other, const time::time_t& t);
 

        class ArrayIterator : std::array<T, Size>::iterator
        {         
            ArrayIterator(const container_t& container, const time::time_t& time = time::TIME_MAX, size_t offset = 0): std::array<T, Size>::iterator(&container.get(time)[0], offset){};
        };


        ArrayIterator begin(const time::time_t &time = time::TIME_MAX) const;

        ArrayIterator end(const time::time_t &time = time::TIME_MAX) const;


    private:
        container_t container;
    };


    
    template <typename T, size_t Size>
    const Keyframe<T>& Array<T, Size>::frame(const time::time_t& t, const size_t index) const
    {
        size_t container_index = container[index].last(t);
        return container[index].get(container_index);
    }

    template <typename T, size_t Size>
    const Keyframe<T>& Array<T, Size>::next_frame(const time::time_t& t, const size_t index) const
    {
        size_t container_index = container[index].last(t);

        return container[index].get(++container_index);
    }
    
    template <typename T, size_t Size>
    T Array<T, Size>::get(const time::time_t& t, const size_t index) const
    {
        return this->frame().value;
    }

    template <typename T, size_t Size>
    std::array<T, Size>  Array<T, Size>::get(const time::time_t& t) const
    {
        return std::array<T, Size>
        {
            []<auto... I>(std::index_sequence<I...>)
            {
                return std::array<T, Size> { this->get(t, I)... };
            }(std::make_index_sequence<Size>{})
        };
    }

    template <typename T, size_t Size>
    size_t Array<T, Size>::size() const
    {
        return size;
    }


    template <typename T, size_t Size>
    void Array<T, Size>::set_insert(const time::time_t& t, const size_t index, T value)
    {
        this->container[index].insert_after(t, value);
    }


    template <typename T, size_t Size>
    void Array<T, Size>::set_last(const time::time_t& t, const size_t index, T value)
    {
        size_t i = this->container[index].insert_after(t, value);
        this->container[index].erase_after(i);
    }


    template <typename T, size_t Size>
    void Array<T, Size>::set_replace(const time::time_t& t, const size_t index, T value)
    {
        size_t i = this->container[index].insert_overwrite(t, value);
    }

    template <typename T, size_t Size>
    void Array<T, Size>::sync(const Array<T, Size>& other, const time::time_t& start)
    {  
        for(int i = 0; i < Size; i++)
        {
            this->container[i].sync(other, start);
        }
    }

    template <typename T, size_t Size>
    Array<T, Size>::ArrayIterator Array<T, Size>::begin(const time::time_t &time) const
    {
        return Array<T, Size>::ArrayIterator(this->container, time);
    }
	

    template <typename T, size_t Size>
    Array<T, Size>::ArrayIterator Array<T, Size>::end(const time::time_t &time) const
    {
        return Array<T, Size>::ArrayIterator(this->container, time, this->container.size);
    }

} //curve
} //openage
