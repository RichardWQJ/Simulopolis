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

#include <memory>
#include <vector>

class RandomGenerator;
class TextFileManager;
class Company;
class Person;

class CompanyGenerator
{
public:
    static void setTextFileManager(TextFileManager* textFileManager);

    CompanyGenerator(RandomGenerator& generator);

    void setUp();

    std::unique_ptr<Company> generate(int year, Person* owner);


private:
    static TextFileManager* sTextFileManager;
    RandomGenerator& mGenerator;
    std::vector<std::string> mFruits;
    std::vector<std::string> mPrefixes;
    std::vector<std::string> mSuffixes;
    std::vector<std::string> mDomains;
};
