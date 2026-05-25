#include <unordered_map>
#include <typeindex>


namespace ECS{
    using Entity = int;

    struct IComponentPool{
        virtual ~IComponentPool() = default;
        virtual void remove(Entity e) = 0; // = 0 MEANS ITS A PURE VIRTUAL FUNCTION EXPECT IMPLEMENTAITION BELOW WEIRD AAH C++ SYNTAX OMGFFG
    };


    template<typename T>
    struct Pool : public IComponentPool{
        constexpr static int MAX_E = 100; 
        std::array<int, MAX_E> sparse{};
        std::array<int, MAX_E> dense{};
        std::array<T, MAX_E> data{};
        int count{};

        Pool() {
            sparse.fill(-1);
        }
                
        void assign(Entity e, T p){
            if (sparse[e] != -1) {
                data[sparse[e]] = p; 
                return;              
            }

            sparse[e] = count;
            dense[count] = e;
            data[count] = p;
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
                data[trash] = data[last];
                sparse[lastEntity] = trash;
            }
            sparse[e] = -1;
            count--;
        }

    };
    class Registry{
        private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools{};
        Entity counter{};
        
        public:
        Entity createEntity(){
            return counter++;
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

        template<typename T>
        void add(Entity e, T componentData){
            getPool<T>().assign(e, componentData);
        }

        void destroy(Entity e){
            for(auto& [type,poolPtr] : pools){
                poolPtr->remove(e);
            }
        }
    };

};