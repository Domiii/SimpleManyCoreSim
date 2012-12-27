#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Message.hpp"
#include "Tile.hpp"

#include <WinBase.h>    //mutex in VS2010
#include <mutex>
#include <deque>


/// A router receives packets from and sends them to immediate neighbors or it's own core
struct Router
{
    Tile* tile;
    std::mutex queueLock;
    std::deque<Message> msgQueue;

    /// Sim stats: How many packets went through this router
    int simTotalPacketsReceived;

    /// Initialize a new Router object
    void InitRouter(Tile* tile);

    // ############################################## Handle messages ##############################################

    /// Called when a Message directed at this tile is dispatched
    void HandleIncomingMessage(Message& msg);

    /**
     * Sends the message with highest priority to it's next target (either this tile's MMU, a neighboring router, the global MMU (, or the entire core block)).
     * Called by Processor.
     */
    void DispatchNext();


    // ############################################## Transport messages ##############################################

    /// Send message to next Tile on the shortest path to target
    void RouteToNeighbor(const Message& msg);

    /// Enqueue message on this router
    void EnqueueMessage(const Message& msg);
};

#endif // ROUTER_HPP
