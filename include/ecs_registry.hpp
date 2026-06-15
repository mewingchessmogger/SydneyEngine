#pragma once  
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <array>
#include <tuple>
#include <variant>
#include <type_traits>
#include "reflections.hpp"

#include "has_member.hpp"



namespace ECS{
    using Entity = int;

    struct IComponentPool{
        virtual ~IComponentPool() = default;
        virtual void remove(Entity e) = 0; // = 0 MEANS ITS A PURE VIRTUAL FUNCTION EXPECT IMPLEMENTAITION BELOW WEIRD AAH C++ SYNTAX OMGFFG
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
            for (int* page : sparse) {
                delete[] page;
            }
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

        // /// @brief ONLY FOR UNIQUE_PTR<sCRIPT> COMPONENTS!!!!!!!
        // /// @param e 
        // /// @param p 
        // void assign(Entity e, T&& p){
        //     if (sparse[e] != -1) {
        //         data[sparse[e]] = std::move(p); 
        //         return;              
        //     }

        //     sparse[e] = count;
        //     dense[count] = e;
        //     data[count] = std::move(p);
        //     count++;
        // }



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

        
        

        std::vector<Variable> getComponentFields(Entity e) override{
            auto& comp = get(e);


            if constexpr (has_member(comp, reflect())){
                return comp.reflect();
            }else{
                return {};
            }


        }

    };
    
    class Registry{
        private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools{};
        //std::unordered_map<std::string, IComponentPool> poolNames{};
        std::vector<Entity> deadIDs{};
        std::vector<Entity> liveIDs{};

        Entity counter{};
        bool registryDirty = false;
        public:

        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>>& getPoolMap(){
            return pools;
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
        std::vector<Entity> getLiveIDs() {
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
            pools[typeKey] = std::make_unique<Pool<T>>();

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

        template<typename... Components>
        std::tuple<Pool<Components>&...> getPools(){
            return std::tie(getPool<Components>()...);
        }



        template<typename... Components>
        void add(Entity e, Components&&... comps) {
            // Perfectly forward each component as an rvalue reference to its pool
            (getPool<std::decay_t<Components>>().assign(e, std::forward<Components>(comps)), ...);
        }


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
    };



};