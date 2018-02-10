#pragma once

// STL
#include <string>
#include <unordered_map>
// SFML
#include <SFML/Graphics/Font.hpp>
// tinyxml2
#include <tinyxml2.h>

/**
 * \brief Manager that manages the fonts
 *
 * \author Pierre Vigier
 */
class FontManager
{
public:
    /**
     * \brief Default constructor
     */
    FontManager();

    /**
     * \brief Destructor
     */
    ~FontManager();

    /**
     * \brief Set up the manager
     *
     * Open the "fonts.xml" file in the folder mPrefixPath and load the
     * fonts specified in this file.
     */
    void setUp();

    /**
     * \brief Tear down the manager
     */
    void tearDown();

    /**
     * \brief Add a font
     *
     * \param name Name of the font
     * \param font Font to add
     */
    void addFont(const std::string& name, sf::Font font);

    /**
     * \brief Get a font
     *
     * \param name Name of the font to retrieve
     *
     * \return Font that corresponds to name
     */
    const sf::Font& getFont(const std::string& name) const;

private:
    std::string mPrefixPath; /**< Path of the folder in which is located "fonts.xml" */
    std::unordered_map<std::string, sf::Font> mFonts; /**< Hash map that contains the fonts */

    /**
     * \brief Load a font from a XML node
     *
     * \param node XML node that describes the font
     */
    void loadFont(tinyxml2::XMLElement* node);
};