
#include "curve/base_curve.h"
#include "curve/keyframe_container.h"

//remember to update docs
namespace openage {
namespace curve {

    template <typename T, size_t size>
    class Array : public BaseCurve<T>
    {
        using container_t = typename std::array<KeyframeContainer<T>, size>;


        Array(const std::shared_ptr<event::EventLoop> &loop,
        size_t id,
        const std::string &idstr = "") :
		EventEntity{loop},
		_id{id},
		_idstr{idstr},
		last_change{time::TIME_ZERO},
		first_element{0} {}

        // prevent accidental copy of queue
        Array(const Array &) = delete;


        T get(const time::time_t& t, size_t index);
        T get(const time::time_t& t, size_t index, size_t hint);

        

        std::array<Keyframe<T>, size> get(const time::time_t& t);
        std::array<Keyframe<T>, size> get(const time::time_t& t, size_t hint);

        
        
        
        private:
        container_t container;
    };
    
    template <typename T, size_t size>
    T Array<T, size>::get(const time::time_t& t, size_t index)
    {
        container[index].get(t);
    }

    template <typename T, size_t size>
    T Array<T, size>::get(const time::time_t& t, size_t index, size_t hint)
    {
        container[index].get(t, hint);
    }

    template <typename T, size_t size>
    std::array<Keyframe<T>, size> get(const time::time_t& t)
    {
        return std::array<Keyframe<T>, size> arr{
            []<auto...I>(std::index_sequence<I...>)
            {
                return std::array<Keyframe<T>, size>{(this->container[I].get(t))...};
            }(std::make_index_sequence<size>{})
        };
    }

    template <typename T, size_t size>
    std::array<Keyframe<T>, size> get(const time::time_t& t, size_t hint)
    {
        return std::array<Keyframe<T>, size> arr{
            []<auto...I>(std::index_sequence<I...>)
            {
                return std::array<Keyframe<T>, size>{(this->container[I].get(t, hint))...};
            }(std::make_index_sequence<size>{})
        };
    }   


} //curve
} //openage