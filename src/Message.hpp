#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "Address.hpp"
#include "Cache.hpp"
#include "Dimension.hpp"

#include <stdint.h>

namespace smcsim {

/// The type of message identifies the purpose of a message
enum MessageType
{
    /// Message contains a requested cache line (Receiver: Tile)
    MessageTypeResponseCacheline = 1,

    /// Message requests a cacheline from off-tile L2 (Receiver: Tile)
    MessageTypeRequestL2,

    /// Message requests a cacheline from memory (Receiver: Main Memory MMU)
    MessageTypeRequestMem,
};

/// A message can be sent through the network for
class Message
{
public:
    /// The type of message
    MessageType type;

    /// The id of the sender (tile or Global MMU)
    Dim2 sender;

    /// The id of the receiver (Tile, core block or Global MMU)
    Dim2 receiver;

    /// The id of the request in the sender that this Message is serving
    int requestId;

    /// The total delay that the current instruction (i.e. memory access) has
    /// cost so far
    int totalDelay;

    Address addr;
    CacheLine cacheLine;

    /// Whether this message is broadcasted to all cores of a core block
    bool IsBroadcast() const;

    /// Whether this message is being sent to another tile (or main memory, if false)
    bool IsReceiverTile() const;
};

} // end namespace smcsim

#endif // MESSAGE_HPP
