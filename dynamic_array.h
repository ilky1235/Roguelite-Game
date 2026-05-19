#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <string>
#include <stdlib.h>
#include "splashkit.h"

class array_full
{
};

template <typename T>

class dynamic_array
{
    int size;
    int capacity;
    T *data;

public:
    dynamic_array(void)
    {
        size = 0;
        capacity = 4;
        data = (T *)malloc(sizeof(T) * capacity);
    }

    ~dynamic_array(void)
    {
        for (int i = 0; i < size; i++)
        {
            data[i].~T();
        }

        free(data);
    }

    dynamic_array(const dynamic_array &other)
    {
        size = other.size;
        capacity = other.capacity;
        data = (T *)malloc(sizeof(T) * capacity);

        for (int i = 0; i < size; i++)
        {
            new (&data[i]) T(other.data[i]);
        }
    }

    int capacity_value(void)
    {
        return capacity;
    }
    int length(void)
    {
        return size;
    }
    void resize()
    {
        capacity *= 2;
        T *new_data = (T *)malloc(sizeof(T) * capacity);

        for (int i = 0; i < size; i++)
        {
            new (&new_data[i]) T(data[i]);
            data[i].~T();
        }
        free(data);

        data = new_data;
    }

    void shrink()
    {
        capacity /= 2;
        T *new_data = (T *)malloc(sizeof(T) * capacity);

        for (int i = 0; i < size; i++)
        {
            new (&new_data[i]) T(data[i]);
            data[i].~T();
        }

        free(data);
        data = new_data;
    }

    void add(T num)
    {
        if (size >= capacity)
        {
            resize();
        }

        new (&data[size]) T(num);
        size++;
    }

    T &get(int index)
    {
        if (index < 0 || index >= size)
        {
            string error = "An error occured as it is an invalid index.";
            throw error;
        }

        return data[index];
    }

    const T &get(int index) const
    {
        if (index < 0 || index >= size)
        {
            string error = "An error occured as it is an invalid index.";
            throw error;
        }

        return data[index];
    }

    void remove(int index)
    {
        if (index < 0 || index >= size)
        {
            string error = "An error occured as it is an invalid index.";
            throw error;
        }

        data[index].~T();

        // If removed element is not the last element in the array
        if (index != size - 1) {
            new (&data[index]) T(data[size-1]);
            // Destroy the original
            data[size-1].~T();
        }
        size--;

        if (size > 0 && size < capacity / 2) {
            shrink();
        }
    }

    void fill(int count, T value)
    {
        if (count < 0 || count + size > capacity)
        {
            string error = "An error has occured.";
            throw error;
        }

        for (int i = 0; i < count; i++)
        {
            add(value);
        }
    }

    T &operator[](int index)
    {
        return get(index);
    }

    const T &operator[](int index) const
    {
        return get(index);
    }
};

#endif