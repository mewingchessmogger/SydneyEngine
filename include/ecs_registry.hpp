#pragma once  
// #include <unordered_map>
// #include <typeindex>
// #include <memory>
// #include <array>
// #include <tuple>
// #include <variant>

#include <type_traits>
#include "reflections.hpp"
#include "has_member.hpp"

// #include <optional>
// #include <stdexcept>
// #include "glm/vec3.hpp"

namespace ECS{
    using Entity = int;
    struct Parent{
        int parentID{};
        uint32_t level{};
    };
    struct IComponentPool{
        virtual ~IComponentPool() = default;
        virtual void remove(Entity e) = 0; // = 0 MEANS ITS A PURE VIRTUAL FUNCTION EXPECT IMPLEMENTAITION BELOW WEIRD AAH C++ SYNTAX OMGFFG
        virtual void reset() = 0;
        virtual void assignComponentFields(Entity e, std::vector<Variable>&& vars) = 0;
        virtual bool hasEntity(Entity e) = 0;
        virtual std::vector<Variable> getComponentFields(Entity e) = 0;

    };

    template<typename T>
    struct Pool : public IComponentPool{
        constexpr static int PAGE_SIZE = 32;
        std::vector<int*> sparse{};
        std::vector<int> dense{};
        std::vector<T> data{};
        int count{};

        ~Pool() {
            reset();
        }

        /// @brief FOR COMPONENTS thata arent unique_ptr<Script>
        /// @param e 
        /// @param p 
        void assign(Entity e, T& p){
            
            int page = e / PAGE_SIZE;
            int offset = e % PAGE_SIZE;

            if (page >= sparse.size()){
                sparse.resize(page + 1, nullptr);
            
            }
            if (sparse[page] ==  nullptr){
                sparse[page] = new int[PAGE_SIZE];
                std::fill_n(sparse[page], PAGE_SIZE, -1);
            }


            if (sparse[page][offset] != -1) {
                data[sparse[page][offset]] = p; // Update existing data
                return;
             }


            if (count >= dense.size()) {
                dense.resize(count + 1);
                data.resize(count + 1);
            }


            sparse[page][offset] = count;
            dense[count] = e;
            data[count] = p;
            count++;

        }

        void assign(Entity e, T&& p) {
            assign(e, p); // Inside here, 'p' now has a name, so it cleanly binds to T& p
        }

        T& get(Entity e){
            int page = e / PAGE_SIZE;
            int offset = e % PAGE_SIZE;

            

            if (page >= sparse.size() || sparse[page] == nullptr){
              throw std::runtime_error ("page not allocated for this");
            }
              
            int i = sparse[page][offset];
            if (i == -1) throw std::runtime_error("entity does not own component");

            
            return data[i];
            


            // int i = sparse[e]; // int i = sparse[i / PAGE_SIZE][i % PAGE_SIZE]
            // return data[i];
        }

        void remove(Entity e) override{
            
            int page = e / PAGE_SIZE;
            int offset = e % PAGE_SIZE;

            if (page >= sparse.size()){
               return;
            }
              
            if (sparse[page][offset] == -1) {
                return; //throw std::runtime_error("THERES NOTHING TO REMOVE");
            }

            int trash = sparse[page][offset];
            int last = count - 1;
            
            if(trash != last){
                Entity lastEntity = dense[last];
                dense[trash] = dense[last];
                data[trash] = std::move(data[last]);
                sparse[lastEntity / PAGE_SIZE][lastEntity % PAGE_SIZE] = trash;
            }
            sparse[page][offset] = -1;
            count--;
        }

        bool hasEntity(Entity e) override{
            
            int page = e / PAGE_SIZE;
            int offset = e % PAGE_SIZE;

            if (page >= sparse.size()){
                return false;
            }

            return sparse[page][offset] !=  -1;

        }

        
        
        void reset() override{
            for (int* page : sparse) {
                if(page != nullptr){
                    delete[] page;
                }
            }

            sparse.clear();
            sparse.shrink_to_fit();

            dense.clear();
            dense.shrink_to_fit();
            data.clear();
            data.shrink_to_fit();
            count = 0;
        }

        std::vector<Variable> getComponentFields(Entity e) override{
            auto& comp = get(e);


            if constexpr (has_member(comp, reflect())){
                return comp.reflect();
            }else{
                return {};
            }


        }
        void assignComponentFields(Entity e, std::vector<Variable>&& vars) override{
           T comp{};
            std::vector<VariableAddress> addresses {};
            if constexpr (has_member(comp, reflectAddress())){ // this check is for rn only making sure tagonly comps can get assigned e.g empty structs
                 addresses = comp.reflectAddress();
            }
            
            for(int i{}; i < addresses.size(); i++){

                VariableAddress& v = addresses[i];
                if(std::holds_alternative<int*>(v.address)){
                    int* ptr = std::get<int*>(v.address);
                    *ptr = std::get<int>(vars[i].var);
                }
                else if(std::holds_alternative<float*>(v.address)){
                    float* ptr = std::get<float*>(v.address);
                    *ptr = std::get<float>(vars[i].var);
                }
                else if(std::holds_alternative<uint32_t*>(v.address)){
                    uint32_t* ptr = std::get<uint32_t*>(v.address);
                    *ptr = std::get<uint32_t>(vars[i].var);
                }

                else if(std::holds_alternative<glm::vec<3,float>*>(v.address)){
                    glm::vec<3,float>* ptr = std::get<glm::vec<3,float>*>(v.address);
                    *ptr = std::get<glm::vec<3,float>>(vars[i].var);
                }else{
                    throw std::runtime_error(" UNSUPPORTED POINTER FOUND IN VARIABLE ADDRESS HOW DID WE GET HERE?!?!?! HMMM");
                }
            }
            assign(e, comp);
        }

    };
    
    class Registry{
        private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools{};
        std::unordered_map<std::string, std::optional<std::type_index>> stringToPool{};
        // std::vector<std::unique_ptr<IComponentPool>> vPools{};
        // std::unordered_map<std::string, uint32_t> stringToVPool{};

        std::vector<Entity> deadIDs{};
        std::vector<Entity> liveIDs{};
        
        Entity counter{};
        int poolCounter{};
        bool registryDirty = false;
        public:

        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>>& getPoolMap(){
            return pools;
        }
        Registry(){
        }

        Entity createEntity(){
            registryDirty = true;

            if (deadIDs.empty()){

                return counter++;
            }
            Entity zombie = deadIDs.back(); //  because sparse only takes ent ID and ID is ever increasing even if we kill ents, we save dead ents and reuse them
            deadIDs.pop_back(); //incredibly simple to implement graveyard only a vecotr, popping here and adding them to grave in registry's destroy func, 
            return zombie;
        }

        /*//interesting tradeoff here, this is called in editor(will do aswell in save/laoding later) ONLY, either you do this 
        or you keep track over live ids gaining overhead in ecs usage but making liveids trivial, isnt this the simpler one?
        if this is called only during editor or saving, do you care about the performance? i dont..
        */
        const std::vector<Entity>& getLiveIDs() {
            if (!registryDirty){
                return liveIDs;
            }
            liveIDs.clear();
            liveIDs.reserve(counter - deadIDs.size());
            for (Entity i{}; i < counter; ++i){
                if(std::find(deadIDs.begin(), deadIDs.end(), i) == deadIDs.end()){
                    liveIDs.push_back(i);
                }
            }
            liveIDs.shrink_to_fit();
            registryDirty = false;
            return liveIDs;
        }
        
        template<typename T>
        void createPool(){
            std::type_index typeKey = std::type_index(typeid(T));

            if (pools.find(typeKey) != pools.end()){
                throw std::runtime_error("POOL ALREADY EXIST!!!");
            } 

            if (stringToPool.find(std::string{typeKey.name()}) != stringToPool.end()){
                throw std::runtime_error("STRING POOL REP ALREADY EXIST!!!");
            } 

            pools[typeKey] = std::make_unique<Pool<T>>();
            stringToPool[std::string{typeKey.name()}] = typeKey;

            // stringToVPool[std::string{typeKey.name()}] = vPools.size();
            // vPools.emplace_back(std::make_unique<Pool<T>>());

        }

        template<typename T>
        Pool<T>& getPool(){
            std::type_index typeKey = std::type_index(typeid(T));

            if (pools.find(typeKey) == pools.end()){
                throw std::runtime_error("POOL DOES NOT EXIST!!!");
            } 

            IComponentPool* purePtr = pools[typeKey].get();
            Pool<T>* specificPoolPtr = static_cast<Pool<T>*>(purePtr); 
            return *specificPoolPtr;
        }


        void deserializeComponent(Entity e, std::string_view sName, std::vector<Variable>&& vars){
            
            
            if(stringToPool.find(std::string{sName}) == stringToPool.end()){
                throw std::runtime_error("POOL DOES  NOT EXIST!");
            }
            auto& typeKey = stringToPool[std::string{sName}];
            if(typeKey.has_value()){
            
            pools[typeKey.value()]->assignComponentFields(e, std::move(vars));
            }else{
                throw std::runtime_error(" OPTIONAL TYPE KEY EXIST BUT EMPTY!!!");
            }
           
        }

        
        template<typename... Components>
        std::tuple<Pool<Components>&...> getPools(){
            return std::tie(getPool<Components>()...);
        }

        

        template<typename... Components>
        void add(Entity e, Components&&... comps) {
            // Perfectly forward each component as an rvalue reference to its pool
            (getPool<std::decay_t<Components>>().assign(e, std::forward<Components>(comps)), ...);
        }

        // void addParent(Entity e, Entity parent){
        //     auto& pool = getPool<Parent>();
        //     if (pool.hasEntity(parent))
        //         pool.assign(e, {parent, pool.get(parent).level + 1});
        //     else
        //         pool.assign(e, {parent, 0});
        // }

        template<typename... Components>
        void emplace(Entity e){
            (getPool<Components>().assign(e, Components{}), ...);
        }

        void destroy(Entity e){
            registryDirty = true;

            for(auto& [type,poolPtr] : pools){
                poolPtr->remove(e);
            }
            deadIDs.push_back(e);
            //push e into dead id
            
        }
        
        template<typename T>
        void destroyPool(){
            std::type_index typeKey = std::type_index(typeid(T));

            if (pools.find(typeKey) == pools.end()){
                throw std::runtime_error("POOL DONT EXIST!!!");
            } 

            pools.erase(typeKey);
            stringToPool.erase(typeKey.name());
        }

    };



};