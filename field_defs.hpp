#ifdef __cplusplus
    #define FIELDS_UBO_BASE \
        uint64_t indxAdress{0};\
        uint64_t vertAdress{0};\
        uint64_t skinnedVertAdress{0}; \
        uint64_t projectionAddress{0}; \
        uint64_t animationAddress{0};


#else

    #define FIELDS_UBO_BASE \
        uint64_t indxAdress;\
        uint64_t vertAdress;\
        uint64_t skinnedVertAdress; \
        uint64_t projectionAddress; \
        uint64_t boneMatAddress;

#endif

