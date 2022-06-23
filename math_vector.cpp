#include <iostream>
#include <fstream>

#include <iterator>

#include <cmath>
#include <functional>
#include <numeric>

#include <system_error>
#include <cerrno>

#include  <type_traits>
#include <cassert>
#include <cstdint>

template<typename T>
concept _Multiplicable = requires(T a, T b) { a.operator*(b); };

template<class T = double_t> class Vector final {

public:
    using Iterator = T*;
    using ConstIterator = T const*;
    using ReverseIterator = std::reverse_iterator<Iterator>;

public:
    Iterator begin();
    Iterator end();
    ReverseIterator rbegin();
    ReverseIterator rend();
    ConstIterator begin() const;
    ConstIterator end() const;

public:
    explicit Vector(size_t _dimension = 1, const T& defValue = T());

    Vector(const Vector& other);

    Vector(const std::initializer_list<T>& iList);

    template<std::input_or_output_iterator I> Vector(I _begin, I _end);

    ~Vector();


    [[nodiscard]] constexpr size_t GetDimension() const noexcept;

    T& operator[] (size_t position);
    const T& operator[] (size_t position) const;

    T& At(size_t position);
    [[nodiscard]] const T& At(size_t position) const;

    void Show() const noexcept;

    void ToFile(const std::string& filename = "Vector.txt") const;
    void FromFile(const std::string& filename = "Vector.txt");

    Vector operator+ (const Vector& other);
    Vector operator- (const Vector& other);

    constexpr T operator* (Vector other) requires _Multiplicable<T>;
    Vector operator* (const T& value) requires _Multiplicable<T>;

    Vector& operator+= (const Vector& other);
    Vector& operator-= (const Vector& other);
    Vector& operator*= (const T& value);
    Vector& operator= (const Vector& other);

    bool operator== (const Vector& other) const;
    auto operator<=> (const Vector& other) const = default;

    template<class U>
    friend std::ostream& operator<< (std::ostream& out, const Vector<U>& vect) noexcept;

    template<class U>
    friend std::ifstream& operator>> (std::ifstream& in, Vector<U>& vect) noexcept;

private:
    void SetEl(const T& value);

private:
    size_t dimension;
    T* vector;
};

template<class T> Vector<T>::Vector(size_t _dimension, const T& defValue)
        : dimension(_dimension), vector(new T[_dimension]) {
    assert(dimension != 0);
    std::fill(begin(), end(), defValue);
}
template<class T> Vector<T>::Vector(const Vector& other)
        : dimension(other.dimension), vector(new T[other.dimension]) {
    std::copy(other.begin(), other.end(), this->begin());
}
template<class T> Vector<T>::Vector(const std::initializer_list<T>& iList)
        : dimension(iList.size()), vector(new T[iList.size()]) {
    std::copy(iList.begin(), iList.end(), this->begin());
}

template<class T>
template<std::input_or_output_iterator I> Vector<T>::Vector(I _begin, I _end)
: dimension(std::distance(_begin, _end)), vector(new T[dimension]) {
std::copy(_begin, _end, this->begin());
}

template<class T> Vector<T>::~Vector() {
    delete[] vector; vector = nullptr;
}

template<class T> typename Vector<T>::Iterator Vector<T>::begin() {
    return vector;
}
template<class T> typename Vector<T>::Iterator Vector<T>::end() {
    return vector + dimension;
}
template<class T> typename Vector<T>::ReverseIterator Vector<T>::rbegin() {
    return ReverseIterator(end());
}
template<class T> typename Vector<T>::ReverseIterator Vector<T>::rend() {
    return ReverseIterator(begin());
}


template<class T> typename Vector<T>::ConstIterator Vector<T>::begin() const {
    return vector;
}
template<class T> typename Vector<T>::ConstIterator Vector<T>::end() const {
    return vector + dimension;
}

template<class T> void Vector<T>::SetEl(const T& value) {
    Vector vect(this->dimension + 1);
    std::copy(begin(), end(), vect.begin());

    *this = vect;
    vector[dimension - 1] = value;
}


template<class T> inline constexpr size_t Vector<T>::GetDimension() const noexcept {
    return dimension;
}



//template<typename T, typename U = double_t>
//inline constexpr std::enable_if_t<std::is_convertible_v<T, U>, U> //GetVectorLength(Vector<T> vect) {
//	return std::sqrt(std::inner_product(vect.begin(), vect.end(), vect.begin(), U{}));
//}


template<typename T> concept _DoubleConvertible = std::is_convertible_v<T, double_t>;
template<_DoubleConvertible T> inline constexpr double_t GetVectorLength(Vector<T> vect) {
    return std::sqrt(std::inner_product(vect.begin(), vect.end(), vect.begin(), 0));
}


template<class T> void Vector<T>::Show() const noexcept {
    for (const auto& el : *this) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}


template<class T> inline T& Vector<T>::operator[] (size_t position) {
    return vector[position];
}

template<class T> inline const T& Vector<T>::operator[] (size_t position) const {
    return vector[position];
}


template<class T> inline T& Vector<T>::At(size_t position) {
    if (position > dimension - 1 && dimension) {
        throw std::out_of_range("Index is out of range");
    }
    return vector[position];
}

template<class T> inline const T& Vector<T>::At(size_t position) const {
    return At(position);
}


template<class T> void Vector<T>::ToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios_base::in | std::ios_base::trunc);
    if (file.is_open()) {
        file << *this;
        file.close();
    } else {
        throw std::system_error(errno, std::generic_category(), "failed to open " + filename);
    }
}


template<class T> void Vector<T>::FromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios_base::out);
    if (file.is_open()) {
        file >> *this;
        file.close();
    } else {
        throw std::system_error(errno, std::generic_category(), "failed to open " + filename);
    }
}


template<class T> Vector<T> Vector<T>::operator+ (const Vector& other) {
    if (this->dimension != other.dimension) {
        throw std::invalid_argument("Wrong second vector dimension");
    }
    std::for_each(begin(), end(), [&, i = 0] (T& el) mutable -> T
    { return el + other[i++]; });
    return *this;
}


template<class T> Vector<T> Vector<T>::operator- (const Vector& other) {
    if (this->dimension != other.dimension) {
        throw std::invalid_argument("Wrong second vector dimension");
    }
    std::for_each(begin(), end(), [&, i = 0](T& el) mutable -> T
    { return el - other[i++]; });
    return *this;
}

template<class T>
constexpr T Vector<T>::operator* (Vector other) requires _Multiplicable<T> {
    if (this->dimension != other.dimension) {
        throw std::invalid_argument("Wrong second vector dimension");
    }
    return std::inner_product(other.begin(), other.end(), this->begin(), T());
}

template<class T>
Vector<T> Vector<T>::operator* (const T& value) requires _Multiplicable<T> {
    std::for_each(begin(), end(), [&value](T& el) -> T { return el * value; });
    return *this;
}

template<class T> Vector<T>& Vector<T>::operator+= (const Vector& other) {
    return *this = *this + other;
}

template<class T> Vector<T>& Vector<T>::operator-= (const Vector& other) {
    return *this = *this - other;
}

template<class T> Vector<T>& Vector<T>::operator*= (const T& value) {
    return *this = *this * value;
}

template<class T> Vector<T>& Vector<T>::operator= (const Vector& other) {
    if (this == &other) {
        return *this;
    }
    this->~Vector();
    this->vector = new T[other.dimension];

    std::copy(other.begin(), other.end(), this->begin());
    return *this;
}

template<class T> inline bool Vector<T>::operator== (const Vector& other) const {
    return (this->dimension == other.dimension
            && std::equal(this->begin(), this->end(), other.begin()));
}

template<class T>
std::ostream& operator<< (std::ostream& out, const Vector<T>& vect) noexcept {
    std::copy(vect.begin(), vect.end(), std::ostream_iterator<T>(out, " "));
    return out;
}

template<class T>
std::ifstream& operator>> (std::ifstream& in, Vector<T>& vect) noexcept {
    T value = T();
    while (in >> value) {
        vect.SetEl(value);
    }
    return in;
}


class Point {

public:
    constexpr Point(size_t _x = 0, size_t _y = 0) : x(_x), y(_y) { }
    ~Point() = default;

    Point operator+ (const Point& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    Point operator- (const Point& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    Point operator* (const Point& other) {
        this->x *= other.x;
        this->y *= other.y;
        return *this;
    }

    Point& operator+= (const Point& other) {
        return *this = *this + other;
    }

    Point& operator-= (const Point& other) {
        return *this = *this - other;
    }

    Point& operator*= (const Point& other) {
        return *this = *this * other;
    }

    operator size_t() const {
        return x + y;
    }


    friend std::ostream& operator<< (std::ostream& out, const Point& point) {
        return (out << point.x << ',' << point.y);
    }
    friend std::ifstream& operator>> (std::ifstream& in, Point& point) {
        in >> point.x >> point.y;
        return in;
    }

private:
    size_t x;
    size_t y;
};


int main(int argc, char **argv) {

    Vector<Point> vect{ Point(60, 70), {}, Point(30, 20),
                        Point(0, 1), Point(89, 45) };
    vect.ToFile();
    vect.FromFile();

    for (const auto& el : Vector<Point>(vect)) {
        std::cout << el << ' ';
    }

    std::cout << std::endl << GetVectorLength(vect) << std::endl;

    vect *= {5, 10};
    auto vect2 = vect;

    vect2 += Vector<Point>(vect.rbegin(), vect.rend());
    vect2.Show();
    std::cout << vect2 * vect << std::endl;

    std::cin.get();
    return 0;
}
