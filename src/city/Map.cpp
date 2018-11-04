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

#include "Map.h"
#include "util/IdManager.h"
#include "resource/TextureManager.h"
#include "city/Road.h"
#include "city/CallForBids.h"
#include "city/Housing.h"
#include "city/Industry.h"
#include "city/Business.h"
#include "city/Service.h"
#include "city/Company.h"
#include "city/Work.h"
#include "city/Lease.h"
#include "city/Good.h"

std::vector<std::unique_ptr<Tile>> Map::sTileAtlas;

Map::Map() : mWidth(0), mHeight(0), mNbSelected(0), mNetwork(mWidth, mHeight)
{

}

void Map::loadTiles()
{
    sTileAtlas.emplace_back(std::make_unique<Tile>("grass", Tile::Type::EMPTY, Tile::Category::GROUND));
    sTileAtlas.emplace_back(std::make_unique<Tile>("grass", Tile::Type::GRASS, Tile::Category::GROUND));
    sTileAtlas.emplace_back(std::make_unique<Tile>("forest", Tile::Type::FOREST, Tile::Category::OBSTACLE));
    sTileAtlas.emplace_back(std::make_unique<Tile>("water", Tile::Type::WATER, Tile::Category::WATER));
    sTileAtlas.emplace_back(std::make_unique<Tile>("dirt", Tile::Type::DIRT, Tile::Category::OBSTACLE));
    sTileAtlas.emplace_back(std::make_unique<CallForBids>("grass", Tile::Type::CFB_HOUSING, sf::Color(167, 125, 83)));
    sTileAtlas.emplace_back(std::make_unique<Housing>("housing", Tile::Type::AFFORDABLE_HOUSING, 3, 3, 0.2f));
    sTileAtlas.emplace_back(std::make_unique<Housing>("housing", Tile::Type::APARTMENT_BUILDING, 2, 2, 0.3f));
    sTileAtlas.emplace_back(std::make_unique<Housing>("housing", Tile::Type::VILLA, 1, 1, 0.4f));
    sTileAtlas.emplace_back(std::make_unique<CallForBids>("grass", Tile::Type::CFB_INDUSTRY, sf::Color(192, 188, 170)));
    sTileAtlas.emplace_back(std::make_unique<Industry>("industry", Tile::Type::FARM, 1, GoodType::NECESSARY, 16, 2, WorkType::FARMER));
    sTileAtlas.emplace_back(std::make_unique<Industry>("industry", Tile::Type::FACTORY, 3, GoodType::NORMAL, 2, 6, WorkType::WORKER));
    sTileAtlas.emplace_back(std::make_unique<Industry>("industry", Tile::Type::WORKSHOP, 2, GoodType::LUXURY, 0.5, 4, WorkType::CRAFTSPERSON));
    sTileAtlas.emplace_back(std::make_unique<CallForBids>("grass", Tile::Type::CFB_BUSINESS, sf::Color(178, 83, 83)));
    sTileAtlas.emplace_back(std::make_unique<Business>("business", Tile::Type::GROCERY, 1, GoodType::NECESSARY, 16, 16, 2, WorkType::GROCER));
    sTileAtlas.emplace_back(std::make_unique<Business>("business", Tile::Type::MALL, 3, GoodType::NORMAL, 12, 2, 6, WorkType::CASHIER));
    sTileAtlas.emplace_back(std::make_unique<Business>("business", Tile::Type::BOUTIQUE, 2, GoodType::LUXURY, 4, 0.5, 4, WorkType::SALESPERSON));
    sTileAtlas.emplace_back(std::make_unique<CallForBids>("grass", Tile::Type::CFB_HOSPITAL, sf::Color(255, 255, 255)));
    sTileAtlas.emplace_back(std::make_unique<Service>("hospital", Tile::Type::HOSPITAL, 2, 1, WorkType::DOCTOR));
    sTileAtlas.emplace_back(std::make_unique<CallForBids>("grass", Tile::Type::CFB_POLICE_STATION, sf::Color(49, 122, 187)));
    sTileAtlas.emplace_back(std::make_unique<Service>("police", Tile::Type::POLICE_STATION, 2, 1, WorkType::POLICE_OFFICER));
    sTileAtlas.emplace_back(std::make_unique<CallForBids>("grass", Tile::Type::CFB_SCHOOL, sf::Color(207, 156, 104)));
    sTileAtlas.emplace_back(std::make_unique<Service>("school", Tile::Type::SCHOOL, 2, 1, WorkType::TEACHER));
    sTileAtlas.emplace_back(std::make_unique<Road>("road", Tile::Type::ROAD_GRASS));
    sTileAtlas.emplace_back(std::make_unique<Road>("road", Tile::Type::ROAD_SIDEWALK));
    sTileAtlas.emplace_back(std::make_unique<Road>("road", Tile::Type::BRIDGE));
}

const Map::TileAtlas& Map::getTileAtlas()
{
    return sTileAtlas;
}

void Map::fromArray(const Array2<Tile::Type>& tiles)
{
    // Reshape map
    mHeight = tiles.getHeight();
    mWidth = tiles.getWidth();
    mTiles.reshape(mHeight, mWidth);
    mNetwork.reshape(mWidth, mHeight);
    // Create tiles
    for (unsigned int i = 0; i < mHeight; ++i)
    {
        for (unsigned int j = 0; j < mWidth; ++j)
            mTiles.set(i, j, createTile(tiles.get(i, j)));

    }
    // Update tiles
    updateTiles();
}

void Map::deselect()
{
    for (std::unique_ptr<Tile>& tile : mTiles.getData())
        tile->setState(Tile::State::DESELECTED);
    mNbSelected = 0;
}

void Map::bulldoze(Tile::Type type, Company& owner, IdManager<Building*>& buildings, std::vector<Id>& buildingsToRemove)
{
    for (unsigned int i = 0; i < mHeight; ++i)
    {
        for (unsigned int j = 0; j < mWidth; ++j)
        {
            if (mTiles.get(i, j)->getState() == Tile::State::SELECTED)
            {
                // Remove the old tile
                if (mTiles.get(i, j)->isRoad())
                    mNetwork.removeRoad(i, j);
                else if (mTiles.get(i, j)->isBuilding())
                {
                    Building* building = static_cast<Building*>(mTiles.get(i, j).get());
                    building->getOwner()->removeBuilding(building);
                    buildingsToRemove.push_back(building->getId());
                }
                // Add the new tile
                mTiles.set(i, j, createTile(type));
                mTiles.get(i, j)->setPosition(sf::Vector2i(j, i), computePosition(i, j));
                if (mTiles.get(i, j)->isRoad())
                    mNetwork.addRoad(i, j);
                else if (mTiles.get(i, j)->isBuilding())
                {
                    Building* building = static_cast<Building*>(mTiles.get(i, j).get());
                    owner.addBuilding(building);
                    Id id = buildings.add(building);
                    building->setId(id);
                }
                // Update neighborhood
                updateNeighborhood(i, j);
            }
        }
    }
    // Update network
    mNetwork.updateComponents();
}

void Map::select(sf::Vector2i start, sf::Vector2i end, Tile::Category mask)
{
    // Swap coordinates if necessary
    if (end.y < start.y)
        std::swap(start.y, end.y);
    if (end.x < start.x)
        std::swap(start.x, end.x);

    // Clamp in range
    start.x = std::max<int>(std::min<int>(start.x, mWidth - 1), 0);
    start.y = std::max<int>(std::min<int>(start.y, mHeight - 1), 0);
    end.x = std::max<int>(std::min<int>(end.x, mWidth - 1), 0);
    end.y = std::max<int>(std::min<int>(end.y, mHeight - 1), 0);

    for (int i = start.y; i <= end.y; ++i)
    {
        for (int j = start.x; j <= end.x; ++j)
        {
            // Check if the tile type is in the whitelist
            const std::unique_ptr<Tile>& tile = mTiles.get(i, j);
            if (any(tile->getCategory() & mask))
            {
                tile->setState(Tile::State::SELECTED);
                ++mNbSelected;
            }
            else
                tile->setState(Tile::State::INVALID);
        }
    }
}

const Network& Map::getNetwork() const
{
    return mNetwork;
}

bool Map::isReachableFrom(const Tile* start, const Tile* end) const
{
    sf::Vector2i startRoad, endRoad;
    mNetwork.getAdjacentRoad(start->getCoordinates().y, start->getCoordinates().x, startRoad);
    mNetwork.getAdjacentRoad(end->getCoordinates().y, end->getCoordinates().x, endRoad);
    return mNetwork.isReachableFrom(startRoad, endRoad);
}

Path Map::getPath(sf::Vector2i start, sf::Vector2i end) const
{
    std::vector<sf::Vector2i> coordinates = mNetwork.getPath(start, end);
    std::vector<Vector2f> points;
    for (std::size_t i = 0; i < coordinates.size(); ++i)
    {
        const sf::Vector2i& coords = coordinates[i];
        // Offset
        constexpr float t = 0.15f;
        const sf::Vector2f xOffset = sf::Vector2f(-0.5f, 0.25f) * Tile::HEIGHT;
        const sf::Vector2f yOffset = sf::Vector2f(-0.5f, -0.25f) * Tile::HEIGHT;
        sf::Vector2i iOffset;
        if (i > 0)
            iOffset += coords - coordinates[i-1];
        if (i < coordinates.size() - 1)
            iOffset += coordinates[i+1] - coords;
        // No move
        float denom;
        if (std::abs(iOffset.x) + std::abs(iOffset.y) == 0)
            denom = 1.0f;
        // Straight line
        else if (std::abs(iOffset.x) == 0 || std::abs(iOffset.y) == 0)
            denom = std::abs(iOffset.x) + std::abs(iOffset.y);
        // Corner
        else
            denom = 0.75f;
        sf::Vector2f offset = t * (xOffset * iOffset.x + yOffset * iOffset.y) / denom;
        points.push_back(computePosition(coords.y, coords.x) + sf::Vector2f(Tile::HEIGHT, Tile::HEIGHT * 0.5f) + offset);
    }
    return Path(points);
}

std::vector<const Building*> Map::getReachableBuildingsAround(const Tile* origin, int radius, Tile::Type type) const
{
    std::vector<const Building*> buildings;
    sf::Vector2i center = origin->getCoordinates();
    for (int i = std::max(0, center.y - radius); i < std::min(static_cast<int>(mHeight), center.y + radius); ++i)
    {
        for (int j = std::max(0, center.x - radius); j < std::min(static_cast<int>(mWidth), center.x + radius); ++j)
        {
            if (mTiles.get(i, j)->getType() == type)
            {
                const Building* building = static_cast<Building*>(mTiles.get(i, j).get());
                if (isReachableFrom(origin, building))
                    buildings.push_back(building);
            }
        }
    }
    return buildings;
}

unsigned int Map::getWidth() const
{
    return mWidth;
}

unsigned int Map::getHeight() const
{
    return mHeight;
}

const Tile* Map::getTile(std::size_t i, std::size_t j) const
{
    return mTiles.get(i, j).get();
}

const Array2<std::unique_ptr<Tile>>& Map::getTiles() const
{
    return mTiles;
}

unsigned int Map::getNbSelected() const
{
    return mNbSelected;
}

sf::Vector2f Map::computePosition(std::size_t i, std::size_t j) const
{
    sf::Vector2f position;
    position.x = (j - i) * Tile::HEIGHT + mWidth * Tile::HEIGHT;
    position.y = (j + i) * Tile::HEIGHT * 0.5f;
    return position;
}

std::unique_ptr<Tile> Map::createTile(Tile::Type type)
{
    return sTileAtlas[static_cast<int>(type)]->clone();
}

void Map::updateTile(int i, int j)
{
    const Tile* neighbors[3][3];
    for (int di = -1; di <= 1; ++di)
    {
        for (int dj = -1; dj <= 1; ++dj)
        {
            if (i + di >= 0 && i + di < static_cast<int>(mHeight) &&
                j + dj >= 0 && j + dj < static_cast<int>(mWidth))
                neighbors[dj + 1][di + 1] = mTiles.get(i + di, j + dj).get();
            else
                neighbors[dj + 1][di + 1] = sTileAtlas[static_cast<int>(Tile::Type::EMPTY)].get();
        }
    }
    mTiles.get(i, j)->updateVariant(neighbors);
}

void Map::updateNeighborhood(int i, int j)
{
    for (int di = -1; di <= 1; ++di)
    {
        for (int dj = -1; dj <= 1; ++dj)
        {
            if (i + di >= 0 && i + di < static_cast<int>(mHeight) &&
                j + dj >= 0 && j + dj < static_cast<int>(mWidth))
                updateTile(i + di, j + dj);
        }
    }
}

void Map::updateTiles()
{
    for (unsigned int i = 0; i < mHeight; ++i)
    {
        for (unsigned int j = 0; j < mWidth; ++j)
        {
            mTiles.get(i, j)->setPosition(sf::Vector2i(j, i), computePosition(i, j));
            updateTile(i, j);
        }
    }
}
