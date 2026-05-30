#ifndef WORLD_H
#define WORLD_H

#include "../include/raylib.h"

const int CHUNK_SIZE = 16;       
const int MAX_WORLD_HEIGHT = 16; 
const int RENDER_DISTANCE = 2;   

// FIX: Explicitly setting the Block IDs so world.cpp can compile them perfectly!
enum BlockType {
    BLOCK_AIR = 0,
    BLOCK_GRASS = 1,
    BLOCK_STONE = 2,
    BLOCK_TNT = 3,
    BLOCK_IRON_ORE = 4
};

struct WorldChunk {
    int chunkX, chunkZ;          
    unsigned char blocks[CHUNK_SIZE][MAX_WORLD_HEIGHT][CHUNK_SIZE];
    bool isLoaded;
};

class World {
private:
    static const int MAX_LOADED_CHUNKS = (RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1);
    WorldChunk chunkPool[MAX_LOADED_CHUNKS];

    Model modelGrass;
    Model modelStone;
    Model modelTnt;
    Model modelIronOre;

    float GetNoise2D(float x, float z);
    void GenerateChunkData(WorldChunk* chunk);

public:
    bool modelsLoaded;

    World();
    ~World() = default;

    void Init();
    void Update(Vector3 playerPos);
    void Draw();
    void Unload();
    
    bool IsInChunkBounds(int x, int y, int z);
    bool IsBlockSolidAt(int x, int y, int z);
    // Add these public function declarations inside your World class in src/world.h:
    void SetBlockAt(int x, int y, int z, unsigned char blockType);
    void ForceTransformRebuild(); // Forces the position cache to refresh immediately when a block changes
};

#endif // WORLD_H