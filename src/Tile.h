#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include "AnimationHandler.h"

enum class TileType{VOID, GRASS, FOREST, WATER, RESIDENTIAL, COMMERCIAL, INDUSTRIAL, ROAD};

std::string tileTypeToStr(TileType type);

class Tile
{
public:
    Tile();
    Tile(unsigned int tileSize, unsigned int height, sf::Texture& texture,
        std::vector<Animation> animations, TileType type, unsigned int cost,
        unsigned int maxPopPerLevel, unsigned int maxLevels);

    void draw(sf::RenderWindow& window, float dt);
    void update();

    sf::Sprite& getSprite();
    TileType& getType();
    unsigned int& getVariant();
    unsigned int* getRegions();
    double& getPopulation();
    float& getStoredGoods();

private:
    AnimationHandler mAnimationHandler;
    sf::Sprite mSprite;
    TileType mType;
    unsigned int mVariant;

    // Region IDs of the tile, tiles in the same region are connected.
    // First is for transport
    unsigned int mRegions[1];
    // Placement cost of the tile
    unsigned int mCost;
    // Current residents / employees
    double mPopulation;
    // Maximum population per growth stage / tile variant
    unsigned int mMaxPopPerLevel;
    // Maximum number of building levels
    unsigned int mMaxLevels;
    // Production output per customer/worker per day, either monetary or goods
    float mProduction;
    // Goods stored
    float mStoredGoods;
};

using TileAtlas = std::map<std::string, Tile>;
