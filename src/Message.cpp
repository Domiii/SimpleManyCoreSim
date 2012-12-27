#include "Message.hpp"

/// Whether this message is broadcasted to all cores of a core block
bool Message::IsBroadcast() const
{
    // We simplify our simulation for now in such a way that there is no broadcasting required
    return false;
}

/// Whether this message is being sent to another tile (or main memory, if false)
bool Message::IsReceiverTile() const
{
    return type != MessageTypeRequestMem;
}
