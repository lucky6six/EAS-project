#ifndef __MY_QUEUE_H__
#define __MY_QUEUE_H__

#include <vector>
#include <stdexcept>

template <typename T>
class Queue {
public:
    class Iterator;

    Queue() = default;
    ~Queue() = default;

    void push(const T& value);
    void pop();
    T front() const;
    bool empty() const;
    size_t size() const;

    Iterator begin();
    Iterator end();

private:
    std::vector<T> data;
};

template <typename T>
class Queue<T>::Iterator {
public:
    Iterator(typename std::vector<T>::iterator it);
    T& operator*();
    T* operator->();
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

private:
    typename std::vector<T>::iterator current;
};

template <typename T>
void Queue<T>::push(const T& value) {
    data.push_back(value);
}

template <typename T>
void Queue<T>::pop() {
    if (!empty()) {
        data.erase(data.begin());
    }
}

template <typename T>
T Queue<T>::front() const {
    if (empty()) {
        throw std::out_of_range("Queue is empty");
    }
    return data.front();
}

template <typename T>
bool Queue<T>::empty() const {
    return data.empty();
}

template <typename T>
size_t Queue<T>::size() const {
    return data.size();
}

template <typename T>
Queue<T>::Iterator::Iterator(typename std::vector<T>::iterator it) : current(it) {}

template <typename T>
T& Queue<T>::Iterator::operator*() {
    return *current;
}

template <typename T>
T* Queue<T>::Iterator::operator->() {
    return &(*current);
}

template <typename T>
typename Queue<T>::Iterator& Queue<T>::Iterator::operator++() {
    ++current;
    return *this;
}

template <typename T>
typename Queue<T>::Iterator Queue<T>::Iterator::operator++(int) {
    Iterator temp = *this;
    ++current;
    return temp;
}

template <typename T>
bool Queue<T>::Iterator::operator==(const Iterator& other) const {
    return current == other.current;
}

template <typename T>
bool Queue<T>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

template <typename T>
typename Queue<T>::Iterator Queue<T>::begin() {
    return Iterator(data.begin());
}

template <typename T>
typename Queue<T>::Iterator Queue<T>::end() {
    return Iterator(data.end());
}

#endif // __MY_QUEUE_H__
