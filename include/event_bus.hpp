#pragma once 
#include "string_hasher.hpp"
#define EVENT_NAME(type) \
    static constexpr std::string_view getName(){\
        return #type; \
    }\
    static constexpr uint64_t getHash(){\
        return Hasher::stringview(#type);\
    }\
    

struct CollisionEvent{
    int self{};
    int other{};
    EVENT_NAME(CollisionEvent);
};

struct Subscribed{
    int ent{-1};
    //void(*callback)(int, int);
    int vTableSlot = -1;
};


class EventBus{
    std::unordered_map<uint64_t, uint32_t> evnts{};
    
    public:
    std::vector<CollisionEvent> collisions{};
    std::vector<Subscribed> subscribedEntities{};
    EventBus() {
        evnts = {{CollisionEvent::getHash(), 0}};
    }
    
    // template<typename EventType>
    // void addEventType(){
    //     evnts[EventType::getHash()] = 0;
    // }

    // /* i increment that eventtype subscription slot so i know theres atleast on subscirber, 
    // i also push back relevant entity and the callback function....*/
    // template<typename EventType>
    // void subscribe(int entity, std::function<void(const void*)> callback){ 
    //     evnts[EventType::getHash()]++;
    //     subscribers.push_back({
    //         .ent = entity, 
    //         .func = [callback](const void* rawEvent){
    //             const EventType& event = *static_cast<const EventType*>(rawEvent);
    //             callback(event);
    //         }
    //     });
    // }
    
    // template<typename EventType>
    // bool hasSubcribers(){
    //     return evnts[EventType::getHash()] > 0;
    // }
 
};