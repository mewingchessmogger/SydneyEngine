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
        constexpr static int MAX_E = 30; 
        std::array<int, MAX_E> sparse{};
        std::array<int, MAX_E> dense{};
        std::array<T, MAX_E> data{};
        int count{};

        Pool() {
            sparse.fill(-1);
        }
        /// @brief FOR COMPONENTS thata arent unique_ptr<Script>
        /// @param e 
        /// @param p 
        void assign(Entity e, T& p){
            if (sparse[e] != -1) {
                data[sparse[e]] = p; 
                return;              
            }

            sparse[e] = count;
            dense[count] = e;
            data[count] = p;
            count++;
        }
        /// @brief ONLY FOR UNIQUE_PTR<sCRIPT> COMPONENTS!!!!!!!
        /// @param e 
        /// @param p 
        void assign(Entity e, T&& p){
            if (sparse[e] != -1) {
                data[sparse[e]] = std::move(p); 
                return;              
            }

            sparse[e] = count;
            dense[count] = e;
            data[count] = std::move(p);
            count++;
        }



        T& get(Entity e){
            int i = sparse[e];
            return data[i];
        }

        void remove(Entity e) override{
            if (sparse[e] == -1) {
                return; //throw std::runtime_error("THERES NOTHING TO REMOVE");
            }

            int trash = sparse[e];
            int last = count - 1;

            if(trash != last){
                Entity lastEntity = dense[last];
                dense[trash] = dense[last];
                data[trash] = std::move(data[last]);
                sparse[lastEntity] = trash;
            }
            sparse[e] = -1;
            count--;
        }

        bool hasEntity(Entity e) override{
            return sparse[e] != -1;
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


        // template<typename... Components>
        // void add(Entity e, Components... comps){
        //     (getPool<Components>().assign(e, comps), ...);
        // }

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