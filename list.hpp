#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>
#include <functional>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        // Constructor for sentinel nodes (no data)
        node() : data(nullptr), prev(nullptr), next(nullptr) {}

        // Constructor with data
        node(const T &value) : prev(nullptr), next(nullptr) {
            data = new T(value);
        }

        // Destructor
        ~node() {
            if (data != nullptr) {
                delete data;
                data = nullptr;
            }
        }
    };

protected:
    node *head;  // sentinel node before the first element
    node *tail;  // sentinel node after the last element
    size_t _size;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }

    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
        friend class list<T>;
        friend class const_iterator;
    private:
        node *ptr;
        const list<T> *container;

    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, const list<T> *c) : ptr(p), container(c) {}

        /**
         * iter++
         */
        iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        /**
         * ++iter
         */
        iterator & operator++() {
            if (ptr == nullptr || ptr->next == nullptr) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        /**
         * iter--
         */
        iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->data == nullptr) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        /**
         * --iter
         */
        iterator & operator--() {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->data == nullptr) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        /**
         * *it
         */
        T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        /**
         * it->field
         */
        T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        /**
         * check whether two iterators are same
         */
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    /**
     * const_iterator
     */
    class const_iterator {
        friend class list<T>;
        friend class iterator;
    private:
        const node *ptr;
        const list<T> *container;

    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(const node *p, const list<T> *c) : ptr(p), container(c) {}
        const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        const_iterator & operator++() {
            if (ptr == nullptr || ptr->next == nullptr) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        const_iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->data == nullptr) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        const_iterator & operator--() {
            if (ptr == nullptr || ptr->prev == nullptr || ptr->prev->data == nullptr) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        const T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        const T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

    /**
     * Constructors
     */
    list() : _size(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
    }

    list(const list &other) : _size(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;

        for (node *cur = other.head->next; cur != other.tail; cur = cur->next) {
            push_back(*(cur->data));
        }
    }

    /**
     * Destructor
     */
    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }

    /**
     * Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;

        clear();
        for (node *cur = other.head->next; cur != other.tail; cur = cur->next) {
            push_back(*(cur->data));
        }
        return *this;
    }

    /**
     * access the first / last element
     */
    const T & front() const {
        if (_size == 0) {
            throw container_is_empty();
        }
        return *(head->next->data);
    }

    const T & back() const {
        if (_size == 0) {
            throw container_is_empty();
        }
        return *(tail->prev->data);
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(head->next, this);
    }

    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }

    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(tail, this);
    }

    const_iterator cend() const {
        return const_iterator(tail, this);
    }

    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        return _size == 0;
    }

    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return _size;
    }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *cur = head->next;
        while (cur != tail) {
            node *next = cur->next;
            delete cur;
            cur = next;
        }
        head->next = tail;
        tail->prev = head;
        _size = 0;
    }

    /**
     * insert value before pos
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) {
            throw invalid_iterator();
        }
        node *new_node = new node(value);
        insert(pos.ptr, new_node);
        _size++;
        return iterator(new_node, this);
    }

    /**
     * remove the element at pos
     */
    virtual iterator erase(iterator pos) {
        if (pos.container != this || pos.ptr == tail || pos.ptr->data == nullptr) {
            throw invalid_iterator();
        }
        if (_size == 0) {
            throw container_is_empty();
        }

        node *next = pos.ptr->next;
        erase(pos.ptr);
        delete pos.ptr;
        _size--;
        return iterator(next, this);
    }

    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        node *new_node = new node(value);
        insert(tail, new_node);
        _size++;
    }

    /**
     * removes the last element
     */
    void pop_back() {
        if (_size == 0) {
            throw container_is_empty();
        }
        node *to_delete = tail->prev;
        erase(to_delete);
        delete to_delete;
        _size--;
    }

    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        node *new_node = new node(value);
        insert(head->next, new_node);
        _size++;
    }

    /**
     * removes the first element.
     */
    void pop_front() {
        if (_size == 0) {
            throw container_is_empty();
        }
        node *to_delete = head->next;
        erase(to_delete);
        delete to_delete;
        _size--;
    }

    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (_size <= 1) return;

        // Copy data to array
        T **arr = new T*[_size];
        node *cur = head->next;
        for (size_t i = 0; i < _size; i++) {
            arr[i] = cur->data;
            cur = cur->next;
        }

        // Sort array
        std::function<bool(T* const &, T* const &)> cmp = [](T* const &a, T* const &b) -> bool {
            return *a < *b;
        };
        sjtu::sort(arr, arr + _size, cmp);

        // Rebuild links
        cur = head->next;
        for (size_t i = 0; i < _size; i++) {
            cur->data = arr[i];
            cur = cur->next;
        }

        delete[] arr;
    }

    /**
     * merge two sorted lists into one
     */
    void merge(list &other) {
        if (this == &other) return;

        node *cur1 = head->next;
        node *cur2 = other.head->next;

        while (cur2 != other.tail) {
            if (cur1 == tail || *(cur2->data) < *(cur1->data)) {
                node *next2 = cur2->next;
                other.erase(cur2);
                insert(cur1, cur2);
                cur2 = next2;
            } else {
                cur1 = cur1->next;
            }
        }

        _size += other._size;
        other._size = 0;
    }

    /**
     * reverse the order of the elements
     */
    void reverse() {
        if (_size <= 1) return;

        node *cur = head->next;
        while (cur != tail) {
            node *next = cur->next;
            node *tmp = cur->prev;
            cur->prev = cur->next;
            cur->next = tmp;
            cur = next;
        }

        // Swap head and tail pointers
        node *tmp = head->next;
        head->next = tail->prev;
        tail->prev = tmp;

        head->next->prev = head;
        tail->prev->next = tail;
    }

    /**
     * remove all consecutive duplicate elements
     */
    void unique() {
        if (_size <= 1) return;

        node *cur = head->next;
        while (cur != tail && cur->next != tail) {
            if (*(cur->data) == *(cur->next->data)) {
                node *to_delete = cur->next;
                erase(to_delete);
                delete to_delete;
                _size--;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
