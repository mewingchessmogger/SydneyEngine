#include <unordered_map>
#include <typeindex>
#include <memory>



enum class PoolDomain{
    Engine, 
    GameDLL,
};

const uint32_t MAX_ENTITY = 100;

using Entity = int;
struct IComponentPool {
    virtual ~IComponentPool() = default;
    virtual void remove(Entity e) = 0;
};


template <typename T>
struct ComponentPool : public IComponentPool{
    std::array<Entity, MAX_ENTITY> sparse{};
    std::array<Entity, MAX_ENTITY> dense{};
    std::array<T, MAX_ENTITY> data{};
    Entity count = 0;
    ComponentPool(){
        std::fill(sparse.begin(), sparse.end(), -1);
    }

    void assign(Entity e, T val){
        sparse[e] = count;
        dense[count] = e;
        data[count] = val;
        count++;
    }

    T& get(Entity e){
        int id = sparse[e];
        if (id < 0 ) throw std::out_of_range("Entity ID is out of bounds for this pool.");

        return data[id];
    }
    
    

    void remove(Entity e){
        //if  count == 0 throw error 
        if(count == 0) throw std::out_of_range("NO values exist to be removed");

        
        int idx = sparse[e];
        int lastIdx = count - 1;
        
        if(lastIdx == idx) {sparse[e] = -1; count--; return;}
        
        int lastEntity = dense[lastIdx];
        data[idx] = data[lastIdx];
        dense[idx] = dense[lastIdx];

        sparse[lastEntity] = idx;

        sparse[e] = -1;
        count--;
    }

};

class Registry {
    
private:
    struct PoolMetaData {
        PoolDomain domain;
        std::unique_ptr<IComponentPool> pool;
    };
    std::unordered_map<std::type_index, PoolMetaData> pools;
    Entity entityCounter = 0;

public:
    Entity create() {
        return entityCounter++;
        
    }

    template<typename T>
    ComponentPool<T>& getPool(PoolDomain domain) {
        PoolMetaData& meta = pools[std::type_index(typeid(T))];

        if(!meta.pool) meta = {domain, std::make_unique<ComponentPool<T>>()};

        auto* rawBasePtr = meta.pool.get();
        auto* derivedPtr = static_cast<ComponentPool<T>*>(rawBasePtr);
        return *derivedPtr;

    }
    
    template<typename T>
    void add(Entity e, T value, PoolDomain targetDomain) {
        getPool<T>(targetDomain).assign(e, value);
    }
    

    
    void clearDomain(PoolDomain targetDomain){
        std::unordered_map<std::type_index, PoolMetaData>::iterator it = pools.begin();
        while(it != pools.end()){
            if(it->second.domain == targetDomain){
                std::cout << "clearing... " << it->first.name();
                it = pools.erase(it);
            }
            else{
                ++it;
            }
        }
        
    }
};