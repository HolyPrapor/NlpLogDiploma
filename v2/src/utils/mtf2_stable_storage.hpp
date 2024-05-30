//
// Created by zeliboba on 5/30/24.
//

#ifndef DIPLOMA_MTF2_STABLE_STORAGE_HPP
#define DIPLOMA_MTF2_STABLE_STORAGE_HPP

#include "mtf2_storage.hpp"

template <typename T>
class MTF2StableStorage : public MTF2Storage<T> {
public:
    MTF2StableStorage(int maxSize, int staticMovementDegree = 1) : MTF2Storage<T>(maxSize, staticMovementDegree),
                                                                   indices(maxSize, staticMovementDegree) {}
    MTF2StableStorage(std::vector<T> elements, int staticMovementDegree = 1) : MTF2StableStorage(elements, elements.size(), staticMovementDegree) {}
    MTF2StableStorage(std::vector<T> elements, int maxSize, int staticMovementDegree = 1) : MTF2Storage<T>(elements, maxSize, staticMovementDegree), indices(elements.size(), maxSize, staticMovementDegree) {
        for (int i = 0; i < elements.size(); i++) {
            indices.PushAndOverflow(i);
        }
    }
    ~MTF2StableStorage() = default;

    std::optional<T> PushAndOverflow(const T& new_element) override {
        std::optional<T> pushed = std::nullopt;
        if (MTF2Storage<T>::Elements.size() < MTF2Storage<T>::maxSize) {
            MTF2Storage<T>::Elements.push_back(new_element);
            indices.PushAndOverflow(MTF2Storage<T>::Elements.size() - 1);
        } else {
            auto insert_location = indices.Elements.back();
            pushed = std::make_optional<T>(MTF2Storage<T>::Elements[insert_location]);
            MTF2Storage<T>::Elements[insert_location] = new_element;
            indices.PushAndOverflow(insert_location);
        }
        return pushed;
    }

    int MoveToFront(typename std::vector<T>::iterator element) override {
        auto index = std::distance(MTF2Storage<T>::Elements.begin(), element);
        indices.MoveToFront(index);
        return index;
    }
private:
    MTF2Storage<int> indices;
};

#endif //DIPLOMA_MTF2_STABLE_STORAGE_HPP
