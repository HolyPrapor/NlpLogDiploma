//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_MTF2_STORAGE_HPP
#define DIPLOMA_MTF2_STORAGE_HPP

#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>

template <typename T>
class MTF2Storage {
public:
    MTF2Storage(std::vector<T> elements) : MTF2Storage(elements, elements.size()) {}
    MTF2Storage(std::vector<T> elements, int maxSize) : Elements(elements), maxSize(maxSize) {}
    std::vector<T> Elements;

    void PushAndOverflow(const T& new_element) {
        if (collection.size() < maxSize) {
            collection.push_back(new_element);
        } else {
            const float insert_location_ratio = 0.75;
            int insert_location = insert_location_ratio * maxSize;
            Elements.insert(Elements.begin() + insert_location, new_element);
            frequency.erase(Elements.pop_back());
        }
        frequency[new_element] = 1;
    }

    void MoveToFront(const T accessed_element) {
        auto elementLocation = Find(accessed_element);
        MoveToFront(elementLocation);
    }

    void MoveToFront(typename std::vector<T>::iterator element) {
        if (element != Elements.begin()) {
            auto freq = frequency[*element];
            auto dist = static_cast<int>(std::distance(Elements.begin(), element));
            move(Elements, dist, std::max(0, dist - freq));
        }
        frequency[*element]++;
    }

    typename std::vector<T>::iterator Find(const T& element) {
        return std::find(Elements.begin(), Elements.end(), element);
    }

private:
    int maxSize;
    std::vector<T> collection;
    std::unordered_map<T, int> frequency;

    void move(std::vector<T>& v, int oldIndex, int newIndex)
    {
        if (oldIndex > newIndex)
            std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
        else
            std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
    }
};

#endif //DIPLOMA_MTF2_STORAGE_HPP
