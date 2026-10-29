#pragma once 
struct CollisionEvent{
    int self{};
    int other{};
};

struct RaycastEvent{
    int ray{};
    int receiver{};
};

struct Subscribed{
    int ent{-1};
    int vTableSlot = -1;
};


class EventBus{
    public:
    std::vector<CollisionEvent> collisions{};
    std::vector<RaycastEvent> raycastHits{};
    
    std::vector<Subscribed> permanentSubscribers{};
    std::vector<Subscribed> transientSubscribers{};
};