//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_NAIVE_MODEL_HPP
#define DIPLOMA_NAIVE_MODEL_HPP

#include "base_model.hpp"

class NaiveModel : public BaseModel {
public:
    NaiveModel();
    explicit NaiveModel(std::vector<int> frequencies);

    void Feed(const Token& next_token) override;
    const Distribution& GetCurrentDistribution() override;
private:
    Distribution distribution_;
};

#endif //DIPLOMA_NAIVE_MODEL_HPP
