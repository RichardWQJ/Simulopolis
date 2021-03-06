/* Simulopolis
 * Copyright (C) 2018 Pierre Vigier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <boost/serialization/access.hpp>
#include "util/NonCopyable.h"
#include "util/NonMovable.h"

class Person;

class GoalEvaluator : public NonCopyable, public NonMovable
{
public:
    GoalEvaluator(float bias);
    virtual ~GoalEvaluator();

    void setBias(float bias);
    virtual float computeDesirability(Person* person) = 0;
    virtual void setGoal(Person* person) = 0;

protected:
    float mBias;

    GoalEvaluator() = default; // Only for serialization

private:
    // Serialization
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar & mBias;
    }
};
