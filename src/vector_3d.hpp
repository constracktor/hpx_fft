 #ifndef vector_3d_H_INCLUDED
#define vector_3d_H_INCLUDED

#include <cstddef>
#include <algorithm> 

#include <iostream>

template<typename T>
struct vector_3d
{
    T* values_;
    std::size_t size_;
    // x then y major format
    std::size_t dim_x_; // First dimension
    std::size_t dim_y_; // Second dimension 
    std::size_t dim_z_: // Third dimension
public:

    using iterator = T*;
    using const_iterator = const T*;

    // default constructor
    vector_3d();
    vector_3d(std::size_t dim_x, std::size_t dim_y, std::size_t dim_z);
    // explicit contructors
    vector_3d(std::size_t dim_x, std::size_t dim_y, std::size_t dim_z, const T& v );
    // copy constructor
    vector_3d(const vector_3d<T>&);
    // move constructor
    vector_3d(vector_3d<T>&&) noexcept;
    // destructor
    ~vector_3d()=default;
    // {
    //     delete [ ]  values_;
    // }
    vector_3d<T>& operator=(vector_3d<T>&);
    vector_3d<T>& operator=(vector_3d<T>&&) noexcept;

    T& operator()(std::size_t i, std::size_t j, std::size_t k);
    const T& operator()(std::size_t i, std::size_t j, std::size_t k) const;
    T& at(std::size_t i, std::size_t j, std::size_t k);
    const T& at(std::size_t i, std::size_t j, std::size_t k) const;
    constexpr T* data() noexcept;
    constexpr const T* data() const noexcept;

    // iterators
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator  cend() const noexcept;

    iterator at_x(std::size_t j, std::size_t k) noexcept;
    const_iterator at_x(std::size_t j, std::size_t k) const noexcept;
    iterator at_y(std::size_t i, std::size_t k) noexcept;
    const_iterator at_y(std::size_t i, std::size_t k) const noexcept;
    iterator at_z(std::size_t i, std::size_t j) noexcept;
    const_iterator at_z(std::size_t i, std::size_t j) const noexcept;


    // size
    std::size_t size() const noexcept;
    std::size_t dim_x() const noexcept;
    std::size_t dim_y() const noexcept;
    std::size_t dim_z() const noexcept;

    // Non-Member Functions
    template<typename H> friend bool operator==(const vector_3d<H>& lhs, const vector_3d<H>& rhs);

    // see https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    friend void swap(vector_3d& first, vector_3d& second)
    {
        std::swap(first.dim_x_, second.dim_x_);
        std::swap(first.dim_y_, second.dim_y_);
        std::swap(first.dim_z_, second.dim_z_);
        std::swap(first.size_, second.size_);
        std::swap(first.values_, second.values_);
    }
}; 

template<typename T>
inline vector_3d<T>::vector_3d()
{
    dim_x_ = 0;
    dim_y_ = 0;
    dim_z_ = 0;
    size_ = 0;
    values_ = nullptr;
}

template<typename T>
inline vector_3d<T>::vector_3d(std::size_t dim_x, std::size_t dim_y, std::size_t dim_z)
{
    dim_x_ = dim_x;
    dim_y_ = dim_y;
    dim_z_ = dim_z;
    size_ = dim_x_ * dim_y_ * dim_z_;

    values_ = new T[size_];

    for(std::size_t i = 0; i < size_; ++i)
        values_[ i ] = T();
}

template<typename T>
inline vector_3d<T>::vector_3d(std::size_t dim_x, std::size_t dim_y, std::size_t dim_z, const T& v)
{
    dim_x_ = dim_x;
    dim_y_ = dim_y;
    dim_z_ = dim_z;
    size_ = dim_x_ * dim_y_ * dim_z_;

    values_ = new T[size_];

    // for(std::size_t i = 0; i < size_; ++i)
    //     values_[ i ] = v;
    std::fill(begin(),end(),v);
}

template<typename T>
inline vector_3d<T>::vector_3d(const vector_3d<T>& src) : 
    dim_x_(src.dim_x_),
    dim_y_(src.dim_y_),
    dim_z_(src.dim_z_),
    size_(src.size_),
    values_(new T[size_])
{
    // for(std::size_t i = 0; i < size_; ++i)
    //     values_[ i ] = src.values_[ i ];
    // note that this is non-throwing
    std::copy(src.begin(), src.end(), begin());
}

template<typename T>
inline vector_3d<T>::vector_3d(vector_3d<T>&& mv) noexcept
{
    swap(*this, mv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline vector_3d<T>& vector_3d<T>::operator=(vector_3d<T>& src)
{
    swap(*this, src);

    return *this;
}

template<typename T>
inline vector_3d<T>& vector_3d<T>::operator=(vector_3d<T>&& mv) noexcept
{
    swap(*this, mv);

    return *this;
}

template<typename T>
inline typename vector_3d<T>::iterator vector_3d<T>::begin() noexcept
{
    return values_;
}

template<typename T>
inline typename vector_3d<T>::const_iterator vector_3d<T>::begin() const noexcept
{
    return values_;
}

template<typename T>
inline typename vector_3d<T>::iterator  vector_3d<T>::end() noexcept
{
    return values_ + size_;
}

template<typename T>
inline typename vector_3d<T>::const_iterator  vector_3d<T>::end() const noexcept
{
    return values_ + size_;
}

template<typename T>
inline typename vector_3d<T>::const_iterator vector_3d<T>::cbegin() const noexcept
{
    return values_;
}

template<typename T>
inline typename vector_3d<T>::const_iterator  vector_3d<T>::cend() const noexcept
{
    return values_ + size_;
}

template<typename T>
inline typename vector_3d<T>::iterator vector_3d<T>::at_p(std::size_t i, std::size_t j, std::size_t k) 
{
    return values_ + j + i * dim_y_ + k * dim_x_ * dim_y_;
}

template<typename T>
inline typename vector_3d<T>::const_iterator vector_3d<T>::at_p(std::size_t i, std::size_t j, std::size_t k) const noexcept
{
    return values_ + j + i * dim_y_ + k * dim_x_ * dim_y_;
}

//////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline T& vector_3d<T>::operator( ) (std::size_t i, std::size_t j, std::size_t k)
{
    return values_[ j + i * dim_y_ + k * dim_x_ * dim_y_ ];
}

template<typename T>
inline T& vector_3d<T>::at (std::size_t i, std::size_t j, std::size_t k)
{
    if(i * dim_y_ + j  >= size_)
        throw std::runtime_error("out of range exception");
    else
        return values_[ j + i * dim_y_ + k * dim_x_ * dim_y_];
}

template<typename T>
inline const T& vector_3d<T>::operator( ) (std::size_t i, std::size_t j, std::size_t k) const
{
    return values_[ j + i * dim_y_ + k * dim_x_ * dim_y_ ];
}

template<typename T>
inline const T& vector_3d<T>::at (std::size_t i, std::size_t j, std::size_t k) const
{
    if(i * dim_y_ + j >= size_)
        throw std::runtime_error("out of range exception");
    else
        return values_[ j + i * dim_y_ + k * dim_x_ * dim_y_ ];
}

template<typename T>
inline constexpr T* vector_3d<T>::data() noexcept
{
    return values_;
}

template<typename T>
inline constexpr const T* vector_3d<T>::data() const noexcept
{
    return values_;
}

template<typename T>
inline std::size_t  vector_3d<T>::size() const noexcept
{
    return size_;
}

template<typename T>
inline std::size_t  vector_3d<T>::dim_x() const noexcept
{
    return dim_x_;
}

template<typename T>
inline std::size_t  vector_3d<T>::dim_y() const noexcept
{
    return dim_y_;
}

template<typename T>
inline std::size_t  vector_3d<T>::dim_z() const noexcept
{
    return dim_z_;
}

template<typename H>
inline bool operator==(const vector_3d<H>& lhs, const vector_3d<H>& rhs)
{
    if(lhs.dim_x_ != rhs.dim_x_ || lhs.dim_y_ != rhs.dim_y_ || lhs.dim_z_ != rhs.dim_z_)
        return false;

    for(std::size_t i = 0; i < lhs.size_; ++i)
        if(lhs.values_[ i ] != rhs.values_[ i ])
            return false;

    return true;
}

#endif // vector_3d_H_INCLUDED