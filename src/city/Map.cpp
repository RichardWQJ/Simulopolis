#include "Map.h"
#include <fstream>
#include "resource/TextureManager.h"

TileAtlas Map::sTileAtlas;

Map::Map() : mWidth(0), mHeight(0), mNumSelected(0)
{
    mNumRegions[0] = 1;
}

Map::Map(const std::string& filename, unsigned int width, unsigned int height) :
    mNumSelected(0)
{
    load(filename, width, height);
}

void Map::loadTiles(const TextureManager& textureManager)
{
    sTileAtlas["grass"] = Tile(1, textureManager.getTexture("grass"),
        {Animation({{sf::IntRect(0, 0, 132, 99), 0.5f}})},
        Tile::Type::GRASS, 50);

    sTileAtlas["forest"] = Tile(1, textureManager.getTexture("forest"),
        {Animation({{sf::IntRect(0, 0, 132, 99), 0.5f}})},
        Tile::Type::FOREST, 100);

    sTileAtlas["water"] = Tile(1, textureManager.getTexture("water"),
        {Animation({{sf::IntRect(0, 0, 132, 99), 0.5f}})},
        Tile::Type::WATER, 0);

    sTileAtlas["residential"] = Tile(2, textureManager.getTexture("residential"),
        {Animation({{sf::IntRect(0, 0, 133, 163), 0.5f}})},
        Tile::Type::RESIDENTIAL, 300);

    sTileAtlas["commercial"] = Tile(2, textureManager.getTexture("commercial"),
        {Animation({{sf::IntRect(0, 0, 132, 163), 0.5f}})},
        Tile::Type::COMMERCIAL, 300);

    sTileAtlas["industrial"] = Tile(2, textureManager.getTexture("industrial"),
        {Animation({{sf::IntRect(0, 0, 132, 163), 0.5f}})},
        Tile::Type::INDUSTRIAL, 300);

    sTileAtlas["road"] = Tile(1, textureManager.getTexture("road"),
        {Animation({{sf::IntRect(0, 0, 132, 99), 0.5f}}), Animation({{sf::IntRect(0, 99, 132, 99), 0.5f}}),
        Animation({{sf::IntRect(0, 198, 132, 99), 0.5f}}), Animation({{sf::IntRect(0, 297, 132, 99), 0.5f}}),
        Animation({{sf::IntRect(0, 396, 132, 99), 0.5f}}), Animation({{sf::IntRect(0, 495, 132, 99), 0.5f}}),
        Animation({{sf::IntRect(0, 594, 132, 99), 0.5f}}), Animation({{sf::IntRect(0, 693, 132, 99), 0.5f}}),
        Animation({{sf::IntRect(0, 792, 132, 99), 0.5f}}), Animation({{sf::IntRect(0, 891, 132, 99), 0.5f}}),
        Animation({{sf::IntRect(0, 990, 132, 99), 0.5f}})},
        Tile::Type::ROAD, 100);
}

TileAtlas& Map::getTileAtlas()
{
    return sTileAtlas;
}

void Map::load(const std::string& filename, unsigned int width, unsigned int height)
{
    std::ifstream inputFile;
    inputFile.open(filename, std::ios::in | std::ios::binary);

    mWidth = width;
    mHeight = height;

    for (unsigned int pos = 0; pos < mWidth * mHeight; ++pos)
    {
        mTileStates.push_back(Tile::State::DESELECTED);

        Tile::Type type;
        inputFile.read((char*)&type, sizeof(type));
        switch (type)
        {
            case Tile::Type::FOREST:
                mTiles.push_back(sTileAtlas.at("forest"));
                break;
            case Tile::Type::WATER:
                mTiles.push_back(sTileAtlas.at("water"));
                break;
            case Tile::Type::RESIDENTIAL:
                mTiles.push_back(sTileAtlas.at("residential"));
                break;
            case Tile::Type::COMMERCIAL:
                mTiles.push_back(sTileAtlas.at("commercial"));
                break;
            case Tile::Type::INDUSTRIAL:
                mTiles.push_back(sTileAtlas.at("industrial"));
                break;
            case Tile::Type::ROAD:
                mTiles.push_back(sTileAtlas.at("road"));
                break;
            default:
                mTiles.push_back(sTileAtlas.at("grass"));
                break;
        }
        Tile& tile = mTiles.back();
        inputFile.read((char*)&tile.getVariant(), sizeof(unsigned int));
        char tmp[4];
        inputFile.read(tmp, sizeof(unsigned int));
        inputFile.read(tmp, sizeof(double));
        inputFile.read(tmp, sizeof(float));
    }

    inputFile.close();
}

void Map::save(const std::string& filename)
{
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out | std::ios::binary);

    for(Tile& tile : mTiles)
    {
        outputFile.write((char*)&tile.getType(), sizeof(Tile::Type));
        outputFile.write((char*)&tile.getVariant(), sizeof(unsigned int));
        /*outputFile.write((char*)tile.getRegions(), sizeof(unsigned int)*1);
        outputFile.write((char*)&tile.getPopulation(), sizeof(double));
        outputFile.write((char*)&tile.getStoredGoods(), sizeof(float));*/
    }

    outputFile.close();
}

void Map::draw(sf::RenderWindow& window, float dt)
{
    for(unsigned int y = 0; y < mHeight; ++y)
    {
        for(unsigned int x = 0; x < mWidth; ++x)
        {
            // Compute the position of the tile in the 2d world
            sf::Vector2f pos;
            pos.x = (x - y) * Tile::SIZE + mWidth * Tile::SIZE;
            pos.y = (x + y) * Tile::SIZE * 0.5f;
            mTiles[y * mWidth + x].getSprite().setPosition(pos);

            // Change the color if the tile is selected
            if(mTileStates[y * mWidth + x] == Tile::State::SELECTED)
                mTiles[y * mWidth + x].getSprite().setColor(sf::Color(0x7d, 0x7d, 0x7d));
            else
                mTiles[y * mWidth + x].getSprite().setColor(sf::Color(0xff, 0xff, 0xff));

            // Draw the tile
            mTiles[y * mWidth + x].draw(window, dt);
        }
    }
    return;
}

/*void Map::findConnectedRegions(std::vector<Tile::Type> whitelist, int regionType)
{
    int label = 1;

    // Reset the label of all tiles
    for (Tile& tile : mTiles)
        tile.getRegions()[regionType] = 0;

    for (unsigned int y = 0; y < mHeight; ++y)
    {
        for (unsigned int x = 0; x < mWidth; ++x)
        {
            // Remove this test?
            bool found = false;
            for (Tile::Type type : whitelist)
            {
                if (type == mTiles[y * mWidth + x].getType())
                {
                    found = true;
                    break;
                }
            }
            if (mTiles[y * mWidth + x].getRegions()[regionType] == 0 && found)
                depthFirstSearch(whitelist, x, y, label++, regionType);
        }
    }
    mNumRegions[regionType] = label;
}*/

void Map::updateDirection(Tile::Type type)
{
    for (unsigned int y = 0; y < mHeight; ++y)
    {
        for (unsigned int x = 0; x < mWidth; ++x)
        {
            int pos = y * mWidth + x;

            if (mTiles[pos].getType() != type)
                continue;

            bool adjacentTiles[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

            // Check for adjacent tiles of the same type
            if (x > 0 && y > 0)
                adjacentTiles[0][0] = (mTiles[(y - 1) * mWidth + (x - 1)].getType() == type);
            if (y > 0)
                adjacentTiles[0][1] = (mTiles[(y - 1) * mWidth + x].getType() == type);
            if (x < mWidth - 1 && y > 0)
                adjacentTiles[0][2] = (mTiles[(y - 1) * mWidth + (x + 1)].getType() == type);
            if (x > 0)
                adjacentTiles[1][0] = (mTiles[y * mWidth + (x-1)].getType() == type);
            if (x < mWidth - 1)
                adjacentTiles[1][2] = (mTiles[y * mWidth + (x + 1)].getType() == type);
            if (x > 0 && y < mHeight - 1)
                adjacentTiles[2][0] = (mTiles[(y + 1) * mWidth + (x - 1)].getType() == type);
            if (y < mHeight - 1)
                adjacentTiles[2][1] = (mTiles[(y + 1) * mWidth + x].getType() == type);
            if (x < mWidth-1 && y < mHeight-1)
                adjacentTiles[2][2] = (mTiles[(y + 1) * mWidth + (x + 1)].getType() == type);

            // Change the tile variant depending on the tile position
            if (adjacentTiles[1][0] && adjacentTiles[1][2] && adjacentTiles[0][1] && adjacentTiles[2][1])
                mTiles[pos].getVariant() = 2;
            else if (adjacentTiles[1][0] && adjacentTiles[1][2] && adjacentTiles[0][1])
                mTiles[pos].getVariant() = 7;
            else if (adjacentTiles[1][0] && adjacentTiles[1][2] && adjacentTiles[2][1])
                mTiles[pos].getVariant() = 8;
            else if (adjacentTiles[0][1] && adjacentTiles[2][1] && adjacentTiles[1][0])
                mTiles[pos].getVariant() = 9;
            else if (adjacentTiles[0][1] && adjacentTiles[2][1] && adjacentTiles[1][2])
                mTiles[pos].getVariant() = 10;
            else if (adjacentTiles[1][0] && adjacentTiles[1][2])
                mTiles[pos].getVariant() = 0;
            else if (adjacentTiles[0][1] && adjacentTiles[2][1])
                mTiles[pos].getVariant() = 1;
            else if (adjacentTiles[2][1] && adjacentTiles[1][0])
                mTiles[pos].getVariant() = 3;
            else if (adjacentTiles[0][1] && adjacentTiles[1][2])
                mTiles[pos].getVariant() = 4;
            else if (adjacentTiles[1][0] && adjacentTiles[0][1])
                mTiles[pos].getVariant() = 5;
            else if (adjacentTiles[2][1] && adjacentTiles[1][2])
                mTiles[pos].getVariant() = 6;
            else if (adjacentTiles[1][0])
                mTiles[pos].getVariant() = 0;
            else if (adjacentTiles[1][2])
                mTiles[pos].getVariant() = 0;
            else if (adjacentTiles[0][1])
                mTiles[pos].getVariant() = 1;
            else if (adjacentTiles[2][1])
                mTiles[pos].getVariant() = 1;
        }
    }
}

void Map::clearSelected()
{
    for (Tile::State& state : mTileStates)
        state = Tile::State::DESELECTED;

    mNumSelected = 0;
}

void Map::select(sf::Vector2i start, sf::Vector2i end, std::vector<Tile::Type> blacklist)
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

    for (int y = start.y; y <= end.y; ++y)
    {
        for (int x = start.x; x <= end.x; ++x)
        {
            // Check if the tile type is in the blacklist. If it is, mark it as
            // invalid, otherwise select it
            Tile::Type type = mTiles[y * mWidth + x].getType();
            if (std::find(blacklist.begin(), blacklist.end(), type) == blacklist.end())
            {
                mTileStates[y * mWidth + x] = Tile::State::SELECTED;
                ++mNumSelected;
            }
            else
                mTileStates[y * mWidth + x] = Tile::State::INVALID;
        }
    }
}

unsigned int Map::getWidth() const
{
    return mWidth;
}

unsigned int Map::getHeight() const
{
    return mHeight;
}

unsigned int Map::getNbTiles() const
{
    return mTiles.size();
}

Tile& Map::getTile(std::size_t position)
{
    return mTiles[position];
}

const Tile& Map::getTile(std::size_t position) const
{
    return mTiles[position];
}

void Map::setTile(std::size_t position, Tile tile)
{
    mTiles[position] = tile;
}

Tile::State Map::getTileState(std::size_t position) const
{
    return mTileStates[position];
}

unsigned int Map::getNumSelected() const
{
    return mNumSelected;
}

/*void Map::depthFirstSearch(std::vector<Tile::Type>& whitelist, int x, int y, int label, int regionType)
{
    // Outside of the map
    if (x < 0 || x >= static_cast<int>(mWidth) || y < 0 || y >= static_cast<int>(mHeight))
        return;

    // Check if the tile is already assigned to a region
    if (mTiles[y * mWidth + x].getRegions()[regionType] != 0)
        return;

    // Check if the type of the tile is in the whitelist
    bool found = false;
    for (Tile::Type type : whitelist)
    {
        if (type == mTiles[y * mWidth + x].getType())
        {
            found = true;
            break;
        }
    }
    if (!found)
        return;

    // Label the tile
    mTiles[y * mWidth + x].getRegions()[regionType] = label;

    // Recursive calls
    depthFirstSearch(whitelist, x - 1, y, label, regionType);
    depthFirstSearch(whitelist, x, y + 1, label, regionType);
    depthFirstSearch(whitelist, x + 1, y, label, regionType);
    depthFirstSearch(whitelist, x, y - 1, label, regionType);
}*/
