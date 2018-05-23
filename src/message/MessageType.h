#pragma once

enum class MessageType
{
    INPUT,
    GUI,
    GUI_WINDOW,
    NEW_GAME,
    LOAD_GAME,
    DISPLAY_MENU,
};

inline std::ostream& operator<<(std::ostream& os, const MessageType& type)
{
    switch (type)
    {
        case MessageType::INPUT:
            os << "input";
            break;
        case MessageType::GUI:
            os << "gui";
            break;
        case MessageType::GUI_WINDOW:
            os << "gui window";
            break;
        case MessageType::NEW_GAME:
            os << "new game";
            break;
        case MessageType::LOAD_GAME:
            os << "load game";
            break;
        default:
            os << "unknown";
            break;
    }
    return os;
}
