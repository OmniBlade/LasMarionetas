/**
 * @file
 *
 * @brief Interface for message servers.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 * 
 * The message server is a system service that allows directors to send
 * messages either to a specific target or to any other director that is
 * subscribed to receive them. The server also allows directors to subscribe
 * and unsubscribe to messages.
 */
#pragma once

#include "igzunknown.h"

static constexpr uint32_t GZIID_cIGZMessageServer = 0xC1085722;

class cGZMessage;
class cIGZMessageTarget;

class cIGZMessageServer : public cIGZUnknown
{
public:
    /**
     * @brief Immediately emits the message given to all subscribers.
     */
    virtual bool MessageSend(const cGZMessage &msg) = 0;
    /**
     * @brief Queues the message for emission on the next tick.
     * @param high_priority Puts the message at the front of the queue.
     */
    virtual bool MessagePost(const cGZMessage &msg, bool high_priority) = 0;
    /**
     * @brief Subscribes a message target to some given event type.
     * @param target The class to subscribe for events.
     * @param msg_id The message type to subscribe to.
     */
    virtual bool AddNotification(cIGZMessageTarget *target, uint32_t msg_id) = 0;
    /**
     * @brief Unsubscribes a message target from some given event type.
     * @param The class to unsubscribe events for.
     * @param msg_id The message type to ignore.
     */
    virtual bool RemoveNotification(cIGZMessageTarget *target, uint32_t msg_id) = 0;
    /**
     * @brief Posts a message to a specific message target.
     */
    virtual bool GeneralMessagePostToTarget(const cGZMessage &msg, cIGZMessageTarget *target) = 0;
    /**
     * @brief Cancels any pending message posts.
     *
     * If any posted messages have not been processed because an active
     * tick has not occurred yet, this method will cancel the messages
     * and prevent them from being sent.
     */
    virtual bool CancelGeneralMessagePostsToTarget(cIGZMessageTarget *target) = 0;
    /**
     * @brief A service event for processing posted messages on each tick.
     */
    virtual bool OnTick() = 0;
    /**
     * @return Size of the message queue.
     */
    virtual uint32_t GetMessageQueueSize() = 0;
    /**
     * @brief Set whether the queue should be cleared each tick or not.
     */
    virtual void SetAlwaysClearQueueOnTick(bool clear) = 0;
};
