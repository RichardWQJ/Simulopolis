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

#include "ImmigrantsWindow.h"
#include "resource/StylesheetManager.h"
#include "gui/Gui.h"
#include "gui/GuiLabel.h"
#include "gui/GuiButton.h"
#include "gui/GuiTable.h"
#include "gui/GuiScrollArea.h"
#include "gui/GuiVBoxLayout.h"
#include "gui/GuiHBoxLayout.h"
#include "city/City.h"
#include "city/Person.h"
#include "city/Market.h"
#include "util/format.h"

ImmigrantsWindow::ImmigrantsWindow(Id listenerId, MessageBus* messageBus, StylesheetManager* stylesheetManager, City& city) :
    GuiWindow("Immigrants", stylesheetManager->getStylesheet("window")), mListenerId(listenerId),
    mMessageBus(messageBus), mStylesheetManager(stylesheetManager), mCity(city),
    mRentalMarket(static_cast<const Market<Lease>*>(mCity.getMarket(MarketType::RENT))),
    mLaborMarket(static_cast<const Market<Work>*>(mCity.getMarket(MarketType::WORK))),
    mTable(nullptr), mRentalMarketLabel(nullptr), mLaborMarketLabel(nullptr), mAttractivenessLabel(nullptr)
{
    mMessageBus->addMailbox(mMailbox);
    mCity.subscribe(mMailbox.getId());
}

ImmigrantsWindow::~ImmigrantsWindow()
{
    mCity.unsubscribe(mMailbox.getId());
    mMessageBus->removeMailbox(mMailbox);
}

void ImmigrantsWindow::setUp()
{
    // Create table
    std::vector<std::string> names{"Name", "Age", "Money", "Visa"};
    mTable = mGui->createWithDefaultName<GuiTable>(names, mStylesheetManager->getStylesheet("table"));

    // Scroll area
    GuiScrollArea* scrollArea = mGui->createWithDefaultName<GuiScrollArea>(sf::Vector2i(400, 200), mStylesheetManager->getStylesheet("scrollarea"));
    scrollArea->add(mTable);
    scrollArea->setLayout(std::make_unique<GuiVBoxLayout>());

    // Labels
    mRentalMarketLabel = mGui->createWithDefaultName<GuiLabel>("", 12, mStylesheetManager->getStylesheet("darkText"));
    mLaborMarketLabel = mGui->createWithDefaultName<GuiLabel>("", 12, mStylesheetManager->getStylesheet("darkText"));
    mAttractivenessLabel = mGui->createWithDefaultName<GuiLabel>("", 12, mStylesheetManager->getStylesheet("darkText"));

    // Buttons
    GuiButton* welcomeAllButton = mGui->create<GuiButton>("welcomeAllButton", mStylesheetManager->getStylesheet("button"));
    welcomeAllButton->setLayout(std::make_unique<GuiHBoxLayout>(0.0f, GuiLayout::Margins{2.0f, 2.0f, 2.0f, 2.0f}));
    welcomeAllButton->add(mGui->createWithDefaultName<GuiLabel>("Accept all", 12, mStylesheetManager->getStylesheet("darkText")));
    welcomeAllButton->subscribe(mListenerId);
    GuiButton* ejectAllButton = mGui->create<GuiButton>("ejectAllButton", mStylesheetManager->getStylesheet("button"));
    ejectAllButton->setLayout(std::make_unique<GuiHBoxLayout>(0.0f, GuiLayout::Margins{2.0f, 2.0f, 2.0f, 2.0f}));
    ejectAllButton->add(mGui->createWithDefaultName<GuiLabel>("Eject all", 12, mStylesheetManager->getStylesheet("darkText")));
    ejectAllButton->subscribe(mListenerId);
    GuiWidget* buttons = mGui->createWithDefaultName<GuiWidget>();
    buttons->setLayout(std::make_unique<GuiHBoxLayout>(GuiLayout::HAlignment::Right, GuiLayout::VAlignment::Center, 4.0f));
    buttons->add(welcomeAllButton);
    buttons->add(ejectAllButton);

    // Window
    add(scrollArea);
    add(mRentalMarketLabel);
    add(mLaborMarketLabel);
    add(mAttractivenessLabel);
    add(buttons);
    setOutsidePosition(sf::Vector2f(50.0f, 50.0f));
    setLayout(std::make_unique<GuiVBoxLayout>(8.0f, GuiLayout::Margins{8.0f, 8.0f, 8.0f, 8.0f}));
    applyStyle();

    // Add rows
    for (Person* immigrant : mCity.getImmigrants())
        addImmigrant(immigrant);

    subscribe(mListenerId);
}

void ImmigrantsWindow::update()
{
    // Read messages
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::CITY)
        {
            const City::Event& event = message.getInfo<City::Event>();
            switch (event.type)
            {
                case City::Event::Type::NEW_IMMIGRANT:
                    addImmigrant(event.person);
                    break;
                case City::Event::Type::IMMIGRANT_EJECTED:
                case City::Event::Type::NEW_CITIZEN:
                    removeImmigrant(event.person);
                    break;
                default:
                    break;
            }
        }
    }

    // Update texts
    mRentalMarketLabel->setString(format("Homes available: %d", mRentalMarket->getItems().size()));
    mLaborMarketLabel->setString(format("Works available: %d", mLaborMarket->getItems().size()));
    mAttractivenessLabel->setString(format("Attractiveness: %.2f", mCity.getAttractiveness()));

    // Update ages
    for (std::size_t i = 0; i < mImmigrants.size(); ++i)
        static_cast<GuiLabel*>(mTable->getCellContent(i, 1))->setString(format("%d", mImmigrants[i]->getAge(mCity.getYear())));
}

void ImmigrantsWindow::addImmigrant(Person* person)
{
    mImmigrants.push_back(person);
    std::string fullName = person->getFullName();

    // Visa
    GuiWidget* visaButtons = mGui->createWithDefaultName<GuiWidget>();
    visaButtons->setLayout(std::make_unique<GuiHBoxLayout>(2.0f));
    GuiButton* acceptedButton = mGui->create<GuiButton>("accepted" + std::to_string(person->getId()) + "|" + visaButtons->getName(), mStylesheetManager->getStylesheet("button"));
    acceptedButton->setLayout(std::make_unique<GuiHBoxLayout>(0.0f, GuiLayout::Margins{2.0f, 2.0f, 2.0f, 2.0f}));
    acceptedButton->add(mGui->createWithDefaultName<GuiLabel>("Yes", 12, mStylesheetManager->getStylesheet("darkText")));
    acceptedButton->subscribe(mListenerId);
    GuiButton* rejectedButton = mGui->create<GuiButton>("rejected" + std::to_string(person->getId()) + "|" + visaButtons->getName(), mStylesheetManager->getStylesheet("button"));
    rejectedButton->add(mGui->createWithDefaultName<GuiLabel>("No", 12, mStylesheetManager->getStylesheet("darkText")));
    rejectedButton->setLayout(std::make_unique<GuiHBoxLayout>(0.0f, GuiLayout::Margins{2.0f, 2.0f, 2.0f, 2.0f}));
    rejectedButton->subscribe(mListenerId);
    visaButtons->add(acceptedButton);
    visaButtons->add(rejectedButton);

    // Add row
    mTable->addRow({
        mGui->createWithDefaultName<GuiLabel>(fullName, 12, mStylesheetManager->getStylesheet("darkText")),
        mGui->createWithDefaultName<GuiLabel>(format("%d", person->getAge(mCity.getYear())), 12, mStylesheetManager->getStylesheet("darkText")),
        mGui->createWithDefaultName<GuiLabel>(format("$%.2f", person->getInitialFunds()), 12, mStylesheetManager->getStylesheet("darkText")),
        visaButtons
    });
}

void ImmigrantsWindow::removeImmigrant(Person* person)
{
    std::size_t i = std::find(mImmigrants.begin(), mImmigrants.end(), person) - mImmigrants.begin();
    mTable->removeRow(i);
    mImmigrants.erase(mImmigrants.begin() + i);
}
