#pragma once

#include <string>
#include <map>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"

class Map
{
public:
    Map();
    Map(const std::string& filename, unsigned int width, unsigned int height, TileAtlas& tileAtlas);

    void load(const std::string& filename, unsigned int width, unsigned int height, TileAtlas& tileAtlas);
    void save(const std::string& filename);

    void draw(sf::RenderWindow& window, float dt);

    void findConnectedRegions(std::vector<TileType> whitelist, int regionType = 0);
    void updateDirection(TileType type);

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned int getTileSize() const;

private:
    unsigned int mWidth;
    unsigned int mHeight;
    std::vector<Tile> mTiles;
    std::vector<int> mResources;
    unsigned int mTileSize;
    unsigned int mSumSelected;
    unsigned int mNumRegions[1];

    void depthFirstSearch(std::vector<TileType>& whitelist, int x, int y, int label, int regionType);
};
