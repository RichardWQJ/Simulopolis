#pragma once

#include <boost/serialization/base_object.hpp>
#include "ai/GoalEvaluator.h"

class GoalEnterCityEvaluator : public GoalEvaluator
{
public:
    GoalEnterCityEvaluator(float bias = 1.0f);

    virtual float computeDesirability(Person* person) override;

    virtual void setGoal(Person* person) override;

private:
    bool mAlreadySelected;

    // Serialization
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar & boost::serialization::base_object<GoalEvaluator>(*this);
        ar & mAlreadySelected;
    }
};
