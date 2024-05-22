//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_MTF2_STORAGE_HPP
#define DIPLOMA_MTF2_STORAGE_HPP

#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <map>

template <typename T>
class MTF2Storage {
public:
    MTF2Storage(int maxSize, int staticMovementDegree = 1) : MTF2Storage(std::vector<T>(), maxSize, staticMovementDegree) {}
    MTF2Storage(std::vector<T> elements, int staticMovementDegree = 1) : MTF2Storage(elements, elements.size(), staticMovementDegree) {}
    MTF2Storage(std::vector<T> elements, int maxSize, int staticMovementDegree = 1) : Elements(elements), maxSize(maxSize), staticMovementDegree(staticMovementDegree) {}
    std::vector<T> Elements;

    void PushAndOverflow(const T& new_element) {
        if (Elements.size() < maxSize) {
            Elements.push_back(new_element);
        } else {
            const float insert_location_ratio = 0.5;
            int insert_location = insert_location_ratio * maxSize;
            Elements.insert(Elements.begin() + insert_location, new_element);
            frequency.erase(Elements.back());
            Elements.pop_back();
        }
        frequency[new_element] = 1;
    }

    int MoveToFrontByIndex(int index) {
        return MoveToFront(Elements.begin() + index);
    }

    int MoveToFront(const T accessed_element) {
        auto elementLocation = Find(accessed_element);
        return MoveToFront(elementLocation);
    }

    int MoveToFront(typename std::vector<T>::iterator element) {
        if (staticMovementDegree == -1) {
            // move the element forward by the number of times it was accessed
            if (element != Elements.begin()) {
                auto freq = frequency[*element];
                auto dist = static_cast<int>(std::distance(Elements.begin(), element));
                auto newIndex = std::max(0, dist - freq);
                move(Elements, dist, newIndex);
                frequency[*element]++;
                return newIndex;
            }
            frequency[*element]++;
            return 0;
        } else {
            // move the element forward every 'staticMovementDegree' time it is accessed
            auto dist = static_cast<int>(std::distance(Elements.begin(), element));
            if (frequency[*element] == staticMovementDegree) {
                frequency[*element] = 0;
                if (element != Elements.begin()) {
                    move(Elements, dist, 0);
                    return 0;
                }
            }
            return dist;
        }
    }

    typename std::vector<T>::iterator Find(const T& element) {
        return std::find(Elements.begin(), Elements.end(), element);
    }

private:
    int maxSize;
    int staticMovementDegree;
    std::map<T, int> frequency;

    void move(std::vector<T>& v, int oldIndex, int newIndex) {
        if (oldIndex < newIndex) {
            throw std::invalid_argument("oldIndex must be greater than or equal to newIndex");
        }

        T temp = std::move(v[oldIndex]); // Store the element to move
        for (int i = oldIndex; i > newIndex; --i) {
            v[i] = std::move(v[i - 1]); // Shift elements to the right
        }
        v[newIndex] = std::move(temp); // Place the element at newIndex
    }
};

#endif //DIPLOMA_MTF2_STORAGE_HPP
