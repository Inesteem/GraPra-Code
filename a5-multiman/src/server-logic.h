#ifndef __SERVER_LOGIC
#define __SERVER_LOGIC

#include <iostream>
#include <list>
#include <unordered_map>

#include "wall-timer.h"
#include "server-networking.h"

using namespace std;

class GameStage;
class GameObject
{
public:
    unsigned int m_x, m_y, m_id;
    GameStage *m_gameStage;

    GameObject(GameStage* gameStage, unsigned int x, unsigned int y, unsigned int id) : m_gameStage(gameStage), m_x(x), m_y(y), m_id(id) {}

};

/*
 *
 *class GraphNode
{
    osm::id_t id;
    vec2f position;

    // a star
    bool open, closed;
    float currentCost;
    float priority;
    GraphNode *pathParent;

    public:
        vec2f GetPosition() { return position; }
        osm::id_t GetId() { return id; }
        vector<GraphEdge> GetEdges() { return edges; }

        GraphNode(osm::id_t id, vec2f position) : id(id), position(position){ }
        void AddEdge(GraphNode *destination, float cost);
        float DistanceToDestination(GraphNode *destination); // for a star heuritic
        float GetCurrentCost() const { return currentCost; }
        void SetCurrentCost(float cost) { currentCost = cost; }
        float GetPriority() const { return priority; }
        void SetPriority(float p) { priority = p; }
        void Reset() { open = false; closed = false; currentCost = 0.0f; priority = 0.0f; pathParent = NULL; }
        void Close() { closed = true; }
        bool IsClosed() { return closed; }
        void Open() { open = true; }
        bool IsOpen() { return open; }
        GraphNode* GetPathParent() { return pathParent; }
        void SetPathParent(GraphNode* parent) { pathParent = parent; }
};


struct NodeCompare
{
    bool operator()(const GraphNode *n1, const GraphNode *n2) const
    {
        return n1->GetPriority() > n2->GetPriority();
    }
};

struct MapPosition
{
    GraphNode *start;
    GraphNode *end;
    float alpha;

    MapPosition(GraphNode *s, GraphNode *e, float a)
        : start(s), end(e), alpha(a) {}
    MapPosition() : start(NULL), end(NULL), alpha(0.0f) {}

    vec2f GetAlphaPosition();
};

class Path
{
    bool

    std::vector<vec2f> nodes;
    vec2f *currentStart;
    vec2f *currentEnd;
    float currentDistance;

    int nodePosition;

    mesh_ref mesh;

public:
    Path() : nodePosition(0) {};

    void AddNodeFront(vec2f vec);
    void CreateMesh();
    void Render();
    vec2f Start();
    bool Advance(float dt, vec2f &pos);
    void ChangeStart(MapPosition startPosition);
    void ChangeEnd(MapPosition endPosition);

};



class Map
{
    unordered_map<id_t,GraphNode*> graphNodes;

    // a star
    priority_queue<GraphNode*, vector<GraphNode*>, NodeCompare> m_openQueue;

    mesh_ref nodeMesh;
    mesh_ref edgeMesh;
    mesh_ref selectionSourceMesh;
    mesh_ref selectionDestinationMesh;
    mesh_ref backgroundMesh;

    Grap        path->ChangeStart(startPosition);
        path->ChangeEnd(endPosition);

        path->CreateMesh();hNode* FindNearestGraphNode(vec2f position);
    vec2f GetSelectedPoint();
    void MakeSelectionMesh();
    void ResetQueue();
    Path *retracePathFromDestination(GraphNode *node);
    void ExpandNode(GraphNode *current,GraphNode *endNode);

    public:
        Map(unordered_map<osm::id_t, vec2f> nodes, unordered_map<osm::id_t, osm::MapElement> mapElements, unordered_map<osm::id_t, osm::Way> ways);
        void Render();
        void RenderDebug();
        void Print();
        Path *FindPathAStar(MapPosition start, MapPosition end);

        vec2f ClickWorldPosition(int x, int y);
        MapPosition GetRandomPosition();
        MapPosition GetNearestEdgeForPosition(vec2f position);
};

*/

struct PathNode
{
    unsigned int mapX, mapY;
};

class Path
{
public:
    list<PathNode> m_nodes;

    Path(PathNode &source, PathNode &destination);
};

class Troup;
class Building : public GameObject
{
        unsigned int m_unitCount;

        wall_time_timer m_generateUnitsTimer;
        const static unsigned int m_unitGenerationTime = 10000;
public: 
    Building(GameStage *gameStage, unsigned int x, unsigned int y, unsigned int id);

    void Update();
    void IncomingTroup(Troup *troup);
    void KillUnits(unsigned int unitCount) { m_unitCount -= unitCount; }
};

class Troup : public GameObject
{
    unsigned int m_stepTime = 500;
    wall_time_timer m_stepTimer;
public:
    unsigned int m_unitCount;

    Building *m_source;
    Building *m_destination;

    Path *m_path;

    Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id);

    bool Update();
};

class GameStage
{
    static unsigned int s_nextBuilding;
    static unsigned int s_nextTroup;
public:
    int m_mapX, m_mapY;
    bool **m_map;

    GameStage() {}

    void init(unsigned int x, unsigned int y);
    void Update();
    Building* spawnHouse(unsigned int x, unsigned int y);
    Troup* spawnTroup(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount);

    unordered_map<unsigned int, Building*> m_buildings;
    unordered_map<unsigned int, Troup*> m_troups;
};

#endif
