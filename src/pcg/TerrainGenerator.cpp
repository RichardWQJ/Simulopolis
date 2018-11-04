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
 
#include "pcg/TerrainGenerator.h"
#include "pcg/noise.h"

TerrainGenerator::TerrainGenerator(RandomGenerator& generator) : mGenerator(generator)
{
    //ctor
}

Array2<Tile::Type> TerrainGenerator::generate() const
{
    unsigned int n = 64;
    double delta = 1.0f / n;
    // Generate noise
    Array2<double> z(n, n);
    for (unsigned int i = 0; i < n; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
            z.set(i, j, fractal_noise_2d(mGenerator.getSeed(), i * delta, j * delta, 6));
    }
    // Normalize
    double minZ = *std::min_element(z.getData().begin(), z.getData().end());
    double maxZ = *std::max_element(z.getData().begin(), z.getData().end());
    for (unsigned int i = 0; i < n; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
            z.set(i, j, (z.get(i, j) - minZ) / (maxZ - minZ));
    }
    // Set tiles
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    double waterThreshold = 0.3;
    double forestThreshold = 0.7;
    double dirtThreshold = 0.05;
    Array2<Tile::Type> tiles(n, n, Tile::Type::GRASS);
    for (unsigned int i = 0; i < n; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
        {
            // Water
            if (z.get(i, j) < waterThreshold)
                tiles.set(i, j, Tile::Type::WATER);
            else
            {
                // Forest
                double p = (z.get(i, j) - waterThreshold) / (1.0 - waterThreshold);
                p = forestThreshold * p * p * p * p;
                if (distribution(mGenerator) < p)
                    tiles.set(i, j, Tile::Type::FOREST);
                // Dirt
                else if (distribution(mGenerator) < dirtThreshold)
                    tiles.set(i, j, Tile::Type::DIRT);
            }
        }
    }
    return tiles;
}
