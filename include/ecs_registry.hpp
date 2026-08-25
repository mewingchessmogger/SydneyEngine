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
        struct Hierarchic{
            int parent = -1;
            uint32_t level{};
            COMP_NAME(Hierarchic);
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
            
            // template<typename Dummy = T>
            // std::enable_if_t<std::is_same_v<Dummy, Hierarchic>> setSparseIndex(Entity e, int denseIndex) {
            //     int page = e / PAGE_SIZE;
            //     int offset = e % PAGE_SIZE;
            //     if (page >= sparse.size() || sparse[page] == nullptr){// will never hpapen but whatever 
            //     throw std::runtime_error ("page not allocated for this");
            //     }
            //     sparse[page][offset] = denseIndex;

            // }
        };
        
        class Registry{
            private:
            std::unordered_map<uint64_t, std::unique_ptr<IComponentPool>> pools{};
            std::unordered_map<std::string, uint64_t> stringToPool{};//std::optional<uint64_t>
            std::vector<Entity> deadIDs{};
            std::vector<Entity> liveIDs{};
            
            Entity counter{};
            int poolCounter{};
            bool registryDirty = false;
            bool isHierarchyPoolDirty = false;
            public:

            std::unordered_map<uint64_t, std::unique_ptr<IComponentPool>>& getPoolMap(){
                return pools;
            }
            Registry(){
                createPool<Hierarchic>();
            }
            void sortHierarchyPool(){
                
                if(!isHierarchyPoolDirty){
                    return;
                }
                
                auto& pool = getPool<Hierarchic>();
                if (pool.dense.size() <= 1){
                    isHierarchyPoolDirty = false;
                    return;
                }
                std::vector<int> index(pool.dense.size(), 0);
                assert(pool.data.size() == pool.dense.size());

                //https://ideone.com/poHXGX
                for(int i {}; i < pool.count;i++){index[i] = i;} //subtle, do not sort dense or data total size, there could be dead data past pool.count, 

                    sort(index.begin(), index.end(), [&](const int& a, const int& b){
                        return (pool.data[a].level < pool.data[b].level);
                    });
                
                    // standard way of swapping, but feels imperfect
                std::vector<Entity> tempDense(pool.dense);
                std::vector<Hierarchic> tempData(pool.data);

                for (int i = 0; i < pool.count; ++i) {
                    int oldIdx = index[i];
                    tempDense[i] = pool.dense[oldIdx];
                    tempData[i] = std::move(pool.data[oldIdx]);
                }
                pool.dense.swap(tempDense);
                pool.data.swap(tempData);

                for (int i = 0; i < pool.count; ++i) {
                    Entity e = pool.dense[i];
                    int page = e / pool.PAGE_SIZE;
                    int offset = e % pool.PAGE_SIZE;
                    
                    pool.sparse[page][offset] = i;

                }
                isHierarchyPoolDirty  = false;
            }

            void setParent(Entity e, Entity parent){
                assert(e >= 0 && parent >= 0);
                
                auto& pool = getPool<Hierarchic>();
                int parentLevel = 1 + (pool.hasEntity(parent) ? pool.get(parent).level : -1);

                pool.assign(e, {parent, static_cast<uint32_t>(parentLevel)});
            
                isHierarchyPoolDirty = true;

            }
            void detachParent(Entity e, bool keepChildren = true){

                auto& pool = getPool<Hierarchic>();
                
                if (!pool.hasEntity(e)) return;

                if (!keepChildren) {
                    std::vector<Entity> stack{e};
                    while(!stack.empty()){
                        Entity curr = stack.back();
                        stack.pop_back();
                        
                        for(int i{}; i < pool.count; i++){
                            if(pool.data[i].parent == curr){
                                stack.push_back(pool.dense[i]);
                            }
                        }
                        pool.remove(curr);
                    }
                }else{
                    pool.remove(e);
                }
                isHierarchyPoolDirty = true;
            }
            void updateHierarchyLevels(){
                if(!isHierarchyPoolDirty) return;
                auto& pool = getPool<Hierarchic>();
                bool changed = true;

                while(changed){
                    changed = false;
                    for(int i {}; i < pool.count; i++){
                        Hierarchic& hier = pool.data[i];        
                        int expectedLevel = 0;
                        if (pool.hasEntity(hier.parent)) {
                            expectedLevel = pool.get(hier.parent).level + 1;
                        }

                        if (hier.level != expectedLevel) {
                            hier.level = expectedLevel;
                            changed = true;
                        }
                    }
                }


            }

            bool getIsHierarchyDirty(){
                return isHierarchyPoolDirty;
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
                if (pools.find(T::getHash()) != pools.end()){
                    throw std::runtime_error("POOL ALREADY EXIST!!!");
                } 

                if (stringToPool.find(std::string{T::getName()}) != stringToPool.end()){
                    throw std::runtime_error("STRING POOL REP ALREADY EXIST!!!");
                } 

                pools[T::getHash()] = std::make_unique<Pool<T>>();
                stringToPool[std::string{T::getName()}] = T::getHash();

            }

            template<typename T>
            Pool<T>& getPool(){
                //printf("%.*s %llu\n", static_cast<int>(T::getName().length()), T::getName().data(), T::getHash());

                if (pools.find(T::getHash()) == pools.end()){
                    throw std::runtime_error("POOL DOES NOT EXIST!!!");
                } 

                IComponentPool* purePtr = pools[T::getHash()].get();
                Pool<T>* specificPoolPtr = static_cast<Pool<T>*>(purePtr); 
                return *specificPoolPtr;
            }
           

            void deserializeComponent(Entity e, std::string_view sName, std::vector<Variable>&& vars){
                
                
                if(stringToPool.find(std::string{sName}) == stringToPool.end()){
                    throw std::runtime_error("POOL DOES  NOT EXIST!");
                }
                
                pools[Hasher::stringview(sName)]->assignComponentFields(e, std::move(vars));
            }

            void deserializeComponent(Entity e, uint64_t compHash, std::vector<Variable>&& vars){
                
                
                if(pools.find(compHash) == pools.end()){
                    throw std::runtime_error("POOL DOES  NOT EXIST!");
                }
                
                pools[compHash]->assignComponentFields(e, std::move(vars));
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
                uint64_t hash = Hierarchic::getHash();
                for(auto& [type,poolPtr] : pools){
                    if (type == hash) {
                        continue; 
                    }
                    poolPtr->remove(e);
                }
                deadIDs.push_back(e);
                //push e into dead id
                
            }
            
            template<typename T>
            void destroyPool(){
                

                if (pools.find(T::getHash()) == pools.end()){
                    throw std::runtime_error("POOL DONT EXIST!!!");
                } 

                pools.erase(T::getHash());
                
                stringToPool.erase(std::string{T::getName()});
            }

        };



    };