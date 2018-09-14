#include "Service.h"
#include "city/City.h"
#include "city/Company.h"
#include "city/Market.h"

Service::Service(const std::string& name, Type type, unsigned int nbStairs, std::size_t nbEmployees,
        Work::Type employeeType) :
    Building(name, type, nbStairs)
{
    for (std::size_t i = 0; i < nbEmployees; ++i)
        mEmployees.emplace_back(std::make_unique<Work>(employeeType, this));
}

Service::~Service()
{
    //dtor
}

std::unique_ptr<Tile> Service::clone() const
{
    return std::make_unique<Service>(mTextureName, mType, mNbStairs, mEmployees.size(), mEmployees.back()->getType());
}

void Service::update()
{
    // Read messages
    while (!mMailbox.isEmpty())
    {
        Message message = mMailbox.get();
        if (message.type == MessageType::MARKET)
        {
            const Market<Work>::Event& event = message.getInfo<const Market<Work>::Event>();
            switch (event.type)
            {
                case Market<Work>::Event::Type::ITEM_ADDED:
                    mWorksInMarket.insert(event.itemId);
                    break;
                case Market<Work>::Event::Type::SALE:
                    mWorksInMarket.erase(event.sale.itemId);
                    break;
                default:
                    break;
            }
        }
    }
}

void Service::tearDown()
{
    update();
    // Remove everything from markets
    const Market<Work>* market = static_cast<const Market<Work>*>(mOwner->getCity()->getMarket(VMarket::Type::WORK));
    for (Id id : mWorksInMarket)
        mOwner->getMessageBus()->send(Message::create(market->getMailboxId(), MessageType::MARKET, market->createRemoveItemEvent(id)));
}

void Service::setOwner(Company* owner)
{
    Building::setOwner(owner);
    for (std::unique_ptr<Work>& employee : mEmployees)
        employee->setEmployer(mOwner);
}

std::vector<std::unique_ptr<Work>>& Service::getEmployees()
{
    return mEmployees;
}

const std::vector<std::unique_ptr<Work>>& Service::getEmployees() const
{
    return mEmployees;
}
