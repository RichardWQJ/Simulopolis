#include "Map.h"
#include <fstream>
#include "resource/TextureManager.h"
#include "city/Road.h"
#include "city/Building.h"

std::vector<std::unique_ptr<Tile>> Map::sTileAtlas;

Map::Map() : mWidth(0), mHeight(0), mNbSelected(0), mNetwork(mWidth, mHeight)
{

}

Map::Map(const std::string& filename, unsigned int width, unsigned int height) :
    mNbSelected(0), mNetwork(width, height)
{
    load(filename, width, height);
}

void Map::loadTiles(const TextureManager& textureManager)
{
    sTileAtlas.push_back(std::unique_ptr<Tile>(new Tile(textureManager.getTexture("grass"),
        Tile::Type::VOID)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Tile(textureManager.getTexture("grass"),
        Tile::Type::GRASS)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Tile(textureManager.getTexture("forest"),
         Tile::Type::FOREST)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Tile(textureManager.getTexture("dirt"),
         Tile::Type::DIRT)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Tile(textureManager.getTexture("water"),
        Tile::Type::WATER)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Building(textureManager.getTexture("residential"),
        Tile::Type::RESIDENTIAL)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Building(textureManager.getTexture("commercial"),
        Tile::Type::COMMERCIAL)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Building(textureManager.getTexture("industrial"),
        Tile::Type::INDUSTRIAL)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Building(textureManager.getTexture("hospital"),
        Tile::Type::HOSPITAL)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Building(textureManager.getTexture("police"),
        Tile::Type::POLICE)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Building(textureManager.getTexture("school"),
        Tile::Type::SCHOOL)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Road(textureManager.getTexture("road"),
        Tile::Type::ROAD_GRASS)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Road(textureManager.getTexture("road"),
        Tile::Type::ROAD_SIDEWALK)));

    sTileAtlas.push_back(std::unique_ptr<Tile>(new Road(textureManager.getTexture("road"),
        Tile::Type::ROAD_WATER)));
}

void Map::load(const std::string& filename, unsigned int width, unsigned int height)
{
    std::ifstream inputFile;
    inputFile.open(filename, std::ios::in | std::ios::binary);

    mWidth = width;
    mHeight = height;

    for (unsigned int i = 0; i < mWidth * mHeight; ++i)
    {
        Tile::Type type;
        inputFile.read((char*)&type, sizeof(type));
        mTiles.push_back(createTile(type));
        mTiles.back()->setPosition(computePosition(mTiles.size() - 1));
        char tmp[4];
        inputFile.read(tmp, sizeof(unsigned int));
        inputFile.read(tmp, sizeof(unsigned int));
        inputFile.read(tmp, sizeof(double));
        inputFile.read(tmp, sizeof(float));
    }

    inputFile.close();

    for (std::size_t i = 0; i < mTiles.size(); ++i)
        updateTile(i);

    mNetwork.resize(mWidth, mHeight);
}

void Map::save(const std::string& filename)
{
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out | std::ios::binary);

    for (const std::unique_ptr<Tile>& tile : mTiles)
    {
        Tile::Type type = tile->getType();
        outputFile.write((char*)&type, sizeof(Tile::Type));
        /*outputFile.write((char*)&tile.getVariant(), sizeof(unsigned int));
        outputFile.write((char*)tile.getRegions(), sizeof(unsigned int)*1);
        outputFile.write((char*)&tile.getPopulation(), sizeof(double));
        outputFile.write((char*)&tile.getStoredGoods(), sizeof(float));*/
    }

    outputFile.close();
}

void Map::deselect()
{
    for (std::unique_ptr<Tile>& tile : mTiles)
        tile->setState(Tile::State::DESELECTED);
    mNbSelected = 0;
}

void Map::bulldoze(Tile::Type type)
{
    for (std::size_t i = 0; i < mTiles.size(); ++i)
    {
        if (mTiles[i]->getState() == Tile::State::SELECTED)
        {
            mTiles[i] = createTile(type);
            mTiles[i]->setPosition(computePosition(i));
            updateNeighborhood(i);
            if (type == Tile::Type::ROAD_GRASS || type == Tile::Type::ROAD_SIDEWALK ||
                type == Tile::Type::ROAD_WATER)
                mNetwork.addRoad(sf::Vector2i(i % mWidth, i / mWidth));
            else if (type == Tile::Type::VOID)
                mNetwork.removeRoad(sf::Vector2i(i % mWidth, i / mWidth));
        }
    }
}

void Map::select(sf::Vector2i start, sf::Vector2i end, const std::vector<Tile::Type>& blacklist)
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
            const std::unique_ptr<Tile>& tile =mTiles[y * mWidth + x];
            if (std::find(blacklist.begin(), blacklist.end(), tile->getType()) == blacklist.end())
            {
                tile->setState(Tile::State::SELECTED);
                ++mNbSelected;
            }
            else
                tile->setState(Tile::State::INVALID);
        }
    }
}

Path Map::getPath(sf::Vector2i start, sf::Vector2i end) const
{
    std::vector<sf::Vector2i> coordinates = mNetwork.getPath(start, end);
    std::vector<Vector2f> points;
    for (std::size_t i = 0; i < coordinates.size(); ++i)
    {
        const sf::Vector2i& coords = coordinates[i];
        // offset
        constexpr float t = 0.15f;
        const sf::Vector2f xOffset = sf::Vector2f(-0.5f, 0.25f) * Tile::SIZE;
        const sf::Vector2f yOffset = sf::Vector2f(-0.5f, -0.25f) * Tile::SIZE;
        sf::Vector2i iOffset;
        if (i > 0)
            iOffset += coords - coordinates[i-1];
        if (i < coordinates.size() - 1)
            iOffset += coordinates[i+1] - coords;
        float denom = std::abs(iOffset.x) == 0 || std::abs(iOffset.y) == 0 ? std::max(std::abs(iOffset.x), std::abs(iOffset.y)) : 0.5f;
        sf::Vector2f offset = t * (xOffset * iOffset.x + yOffset * iOffset.y) / denom;
        //std::cout << iOffset.x << " " << iOffset.y << " " << denom << " " << offset << std::endl;
        points.push_back(computePosition(coords.x + coords.y * mWidth) + sf::Vector2f(Tile::SIZE, Tile::SIZE * 0.5f) + offset);
    }
    return Path(points);
}

unsigned int Map::getWidth() const
{
    return mWidth;
}

unsigned int Map::getHeight() const
{
    return mHeight;
}

const std::vector<std::unique_ptr<Tile>>& Map::getTiles() const
{
    return mTiles;
}

unsigned int Map::getNbSelected() const
{
    return mNbSelected;
}

std::unique_ptr<Tile> Map::createTile(Tile::Type type)
{
    return sTileAtlas[static_cast<int>(type)]->clone();
}

sf::Vector2f Map::computePosition(std::size_t i) const
{
    int x = i % mWidth;
    int y = i / mWidth;
    sf::Vector2f position;
    position.x = (x - y) * Tile::SIZE + mWidth * Tile::SIZE;
    position.y = (x + y) * Tile::SIZE * 0.5f;
    return position;
}

void Map::updateTile(int i)
{
    Tile* neighbors[3][3];
    for (int dy = 0; dy < 3; ++dy)
    {
        for (int dx = 0; dx < 3; ++dx)
        {
            int iNeighbor = i + (dy - 1) * mWidth + (dx - 1);
            if (iNeighbor >= 0 && static_cast<std::size_t>(iNeighbor) < mTiles.size())
                neighbors[dx][dy] = mTiles[iNeighbor].get();
            else
                neighbors[dx][dy] = sTileAtlas[static_cast<int>(Tile::Type::VOID)].get();
        }
    }
    if (mTiles[i]->updateVariant(neighbors))
        updateNeighborhood(i);
}

void Map::updateNeighborhood(std::size_t i)
{
    for (int dy = 0; dy < 3; ++dy)
    {
        for (int dx = 0; dx < 3; ++dx)
        {
            int iNeighbor = i + (dy - 1) * mWidth + (dx - 1);
            if (iNeighbor >= 0 && static_cast<std::size_t>(iNeighbor) < mTiles.size())
                updateTile(iNeighbor);
        }
    }
}
