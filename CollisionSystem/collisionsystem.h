#pragma once

#include "../Entity/entity.h"
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <algorithm>

enum class CollisionSide {
    None,
    Top,
    Bottom,
    Left,
    Right
};

struct CollisionInfo_t {
    Entity* a;
    Entity* b;
    bool staticA;
    bool staticB;
    Vector2 penetration;
    CollisionSide sideA;
    CollisionSide sideB;
};

struct TraceResult_t {
    bool hit;
    Entity* hitEntity;
    Vector2 hitPoint;
    Vector2 hitNormal;
    float distance;
    CollisionSide side;
    
    TraceResult_t() : hit(false), hitEntity(nullptr), 
                    hitPoint({0,0}), hitNormal({0,0}), 
                    distance(INFINITY), side(CollisionSide::None) {}
};

class CollisionSystem {
    CollisionSystem(const CollisionSystem&) = delete;
    CollisionSystem& operator=(const CollisionSystem&) = delete;
    CollisionSystem() : cellSize(100.0f) {}

    // Spatial hash grid
    float cellSize;
    std::unordered_map<int, std::vector<Entity*>> spatialGrid;

    // Hash function for grid cells
    int HashCell(int x, int y) const {
        // Use prime numbers for better distribution
        return (x * 73856093) ^ (y * 19349663);
    }

    // Get all cell keys an entity occupies
    std::vector<int> GetCellKeys(Entity* e) {
        Vector2 minPos, maxPos;
        GetWorldAABB(e, minPos, maxPos);

        int minX = (int)std::floor(minPos.x / cellSize);
        int maxX = (int)std::floor(maxPos.x / cellSize);
        int minY = (int)std::floor(minPos.y / cellSize);
        int maxY = (int)std::floor(maxPos.y / cellSize);

        std::vector<int> keys;
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                keys.push_back(HashCell(x, y));
            }
        }
        return keys;
    }

    CollisionSide Opposite(CollisionSide side) {
        switch (side) {
            case CollisionSide::Top: return CollisionSide::Bottom;
            case CollisionSide::Bottom: return CollisionSide::Top;
            case CollisionSide::Left: return CollisionSide::Right;
            case CollisionSide::Right: return CollisionSide::Left;
            default: return CollisionSide::None;
        }
    }

public:
    static CollisionSystem& GetInstance() {
        static CollisionSystem instance;
        return instance;
    }

    // Set the cell size (should match typical entity size or slightly larger)
    void SetCellSize(float size) {
        cellSize = size;
    }

    float GetCellSize() const {
        return cellSize;
    }

    void GetWorldAABB(Entity* e, Vector2 &minOut, Vector2 &maxOut) {
        Vector2 pos = e->GetPosition();
        Vector2 size = e->GetSize();
        float s = e->GetScale();
        Vector2 half = (size * s) * 0.5f;
        minOut = { pos.x - half.x, pos.y - half.y };
        maxOut = { pos.x + half.x, pos.y + half.y };
    }

    bool Intersect(Entity* a, Entity* b) {
        Vector2 minA, maxA, minB, maxB;
        GetWorldAABB(a, minA, maxA);
        GetWorldAABB(b, minB, maxB);
        return (minA.x < maxB.x && maxA.x > minB.x) && (minA.y < maxB.y && maxA.y > minB.y);
    }

    Vector2 GetPenetrationDepth(Entity* a, Entity* b) {
        Vector2 minA, maxA, minB, maxB;
        GetWorldAABB(a, minA, maxA);
        GetWorldAABB(b, minB, maxB);

        float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
        float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
        if (overlapX < 0.0f || overlapY < 0.0f) return {0.0f, 0.0f};
        return { overlapX, overlapY };
    }

    CollisionSide DetermineCollisionSide(Entity* a, Entity* b, const Vector2& penetration) {
        Vector2 posA = a->GetPosition();
        Vector2 posB = b->GetPosition();

        if (penetration.x < penetration.y) {
            return (posA.x < posB.x) ? CollisionSide::Right : CollisionSide::Left;
        } else {
            return (posA.y < posB.y) ? CollisionSide::Bottom : CollisionSide::Top;
        }
    }

    // Build spatial grid from entities
    void BuildSpatialGrid(const std::vector<Entity*>& entities) {
        spatialGrid.clear();
        
        for (Entity* e : entities) {
            std::vector<int> keys = GetCellKeys(e);
            for (int key : keys) {
                spatialGrid[key].push_back(e);
            }
        }
    }

    // Optimized collision detection using spatial hash
    std::vector<CollisionInfo_t> DetectCollisions(const std::vector<Entity*>& entities) {
        // Build spatial grid
        BuildSpatialGrid(entities);

        std::vector<CollisionInfo_t> out;
        std::unordered_map<Entity*, std::unordered_map<Entity*, bool>> checked;

        // Check collisions only within same cells
        for (auto& [key, cellEntities] : spatialGrid) {
            for (size_t i = 0; i < cellEntities.size(); ++i) {
                for (size_t j = i + 1; j < cellEntities.size(); ++j) {
                    Entity* A = cellEntities[i];
                    Entity* B = cellEntities[j];

                    // Avoid duplicate checks (entities can be in multiple cells)
                    if (checked[A][B] || checked[B][A])
                        continue;
                    checked[A][B] = true;

                    if (Intersect(A, B)) {
                        Vector2 pen = GetPenetrationDepth(A, B);
                        CollisionSide sideA = DetermineCollisionSide(A, B, pen);
                        CollisionSide sideB = Opposite(sideA);

                        out.push_back({
                            A, B,
                            A->IsStatic(), B->IsStatic(),
                            pen,
                            sideA, sideB
                        });
                    }
                }
            }
        }
        return out;
    }

    // Legacy brute-force method (keep for comparison or small entity counts)
    std::vector<CollisionInfo_t> DetectCollisionsBruteForce(const std::vector<Entity*>& entities) {
        std::vector<CollisionInfo_t> out;
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                Entity* A = entities[i];
                Entity* B = entities[j];
                if (Intersect(A, B)) {
                    Vector2 pen = GetPenetrationDepth(A, B);
                    CollisionSide sideA = DetermineCollisionSide(A, B, pen);
                    CollisionSide sideB = Opposite(sideA);

                    out.push_back({
                        A, B,
                        A->IsStatic(), B->IsStatic(),
                        pen,
                        sideA, sideB
                    });
                }
            }
        }
        return out;
    }

    void ResolveCollisions(std::vector<CollisionInfo_t>& collisions) {
        for (auto& c : collisions) {
            if (c.staticA && c.staticB)
                continue;

            Vector2 moveA = { 0.0f, 0.0f };
            Vector2 moveB = { 0.0f, 0.0f };

            if (c.penetration.x < c.penetration.y) {
                if (c.sideA == CollisionSide::Right) {
                    moveA.x = -c.penetration.x;
                    moveB.x =  c.penetration.x;
                } else if (c.sideA == CollisionSide::Left) {
                    moveA.x =  c.penetration.x;
                    moveB.x = -c.penetration.x;
                }
            } else {
                if (c.sideA == CollisionSide::Bottom) {
                    moveA.y = -c.penetration.y;
                    moveB.y =  c.penetration.y;
                } else if (c.sideA == CollisionSide::Top) {
                    moveA.y =  c.penetration.y;
                    moveB.y = -c.penetration.y;
                }
            }

            if (!c.staticA && c.staticB) {
                Vector2 posA = c.a->GetPosition();
                posA += moveA;
                c.a->SetPosition(posA);
            } else if (c.staticA && !c.staticB) {
                Vector2 posB = c.b->GetPosition();
                posB += moveB;
                c.b->SetPosition(posB);
            } else if (!c.staticA && !c.staticB) {
                Vector2 posA = c.a->GetPosition();
                Vector2 posB = c.b->GetPosition();
                posA += moveA * 0.5f;
                posB += moveB * 0.5f;
                c.a->SetPosition(posA);
                c.b->SetPosition(posB);
            }
        }
    }

    // Utility: Get statistics for debugging
    struct Stats {
        int totalCells;
        int totalEntries;
        int maxEntitiesPerCell;
        float avgEntitiesPerCell;
    };

    Stats GetGridStats() const {
        Stats s = {0, 0, 0, 0.0f};
        s.totalCells = spatialGrid.size();
        
        for (const auto& [key, entities] : spatialGrid) {
            s.totalEntries += entities.size();
            s.maxEntitiesPerCell = std::max(s.maxEntitiesPerCell, (int)entities.size());
        }
        
        if (s.totalCells > 0) {
            s.avgEntitiesPerCell = (float)s.totalEntries / (float)s.totalCells;
        }
        
        return s;
    }

    // TraceLine: Cast a ray from start to end, return first hit
    TraceResult_t TraceLine(Vector2 start, Vector2 end, Entity* ignore = nullptr) {
        TraceResult_t result;
        
        // Collect candidate entities along the line path
        std::unordered_set<Entity*> candidates;
        
        Vector2 dir = { end.x - start.x, end.y - start.y };
        float lineLength = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        
        if (lineLength < 0.001f) return result; // Zero-length line
        
        // Normalize direction
        Vector2 normDir = { dir.x / lineLength, dir.y / lineLength };
        
        // Walk along the line, sampling cells
        float step = cellSize * 0.5f;
        int numSteps = (int)(lineLength / step) + 1;
        
        for (int i = 0; i <= numSteps; ++i) {
            float t = std::min((float)i * step, lineLength);
            Vector2 samplePos = { start.x + normDir.x * t, start.y + normDir.y * t };
            
            int cellX = (int)std::floor(samplePos.x / cellSize);
            int cellY = (int)std::floor(samplePos.y / cellSize);
            int key = HashCell(cellX, cellY);
            
            if (spatialGrid.count(key)) {
                for (Entity* e : spatialGrid[key]) {
                    if (e != ignore) {
                        candidates.insert(e);
                    }
                }
            }
        }
        
        // Test line against each candidate AABB
        for (Entity* e : candidates) {
            Vector2 minAABB, maxAABB;
            GetWorldAABB(e, minAABB, maxAABB);
            
            float tMin = 0.0f;
            float tMax = 1.0f;
            
            // Slab test for X axis
            if (std::abs(dir.x) > 0.0001f) {
                float t1 = (minAABB.x - start.x) / dir.x;
                float t2 = (maxAABB.x - start.x) / dir.x;
                tMin = std::max(tMin, std::min(t1, t2));
                tMax = std::min(tMax, std::max(t1, t2));
            } else {
                // Line parallel to X axis
                if (start.x < minAABB.x || start.x > maxAABB.x) {
                    continue; // No intersection
                }
            }
            
            // Slab test for Y axis
            if (std::abs(dir.y) > 0.0001f) {
                float t1 = (minAABB.y - start.y) / dir.y;
                float t2 = (maxAABB.y - start.y) / dir.y;
                tMin = std::max(tMin, std::min(t1, t2));
                tMax = std::min(tMax, std::max(t1, t2));
            } else {
                // Line parallel to Y axis
                if (start.y < minAABB.y || start.y > maxAABB.y) {
                    continue; // No intersection
                }
            }
            
            // Check if intersection exists and is within line segment
            if (tMin <= tMax && tMin >= 0.0f && tMin <= 1.0f) {
                float dist = tMin * lineLength;
                
                // Keep closest hit
                if (dist < result.distance) {
                    result.hit = true;
                    result.hitEntity = e;
                    result.distance = dist;
                    result.hitPoint = { 
                        start.x + dir.x * tMin, 
                        start.y + dir.y * tMin 
                    };
                    
                    // Determine which side was hit and calculate normal
                    Vector2 center = e->GetPosition();
                    Vector2 localHit = {
                        result.hitPoint.x - center.x,
                        result.hitPoint.y - center.y
                    };
                    
                    Vector2 size = e->GetSize() * e->GetScale();
                    float halfW = size.x * 0.5f;
                    float halfH = size.y * 0.5f;
                    
                    // Determine which edge is closest
                    float distToTop = std::abs(localHit.y - halfH);
                    float distToBottom = std::abs(localHit.y + halfH);
                    float distToLeft = std::abs(localHit.x + halfW);
                    float distToRight = std::abs(localHit.x - halfW);
                    
                    float minEdgeDist = std::min({distToTop, distToBottom, distToLeft, distToRight});
                    
                    if (minEdgeDist == distToTop) {
                        result.side = CollisionSide::Top;
                        result.hitNormal = {0.0f, 1.0f};
                    } else if (minEdgeDist == distToBottom) {
                        result.side = CollisionSide::Bottom;
                        result.hitNormal = {0.0f, -1.0f};
                    } else if (minEdgeDist == distToLeft) {
                        result.side = CollisionSide::Left;
                        result.hitNormal = {-1.0f, 0.0f};
                    } else {
                        result.side = CollisionSide::Right;
                        result.hitNormal = {1.0f, 0.0f};
                    }
                }
            }
        }
        
        return result;
    }

    // TraceLine with explicit entity list (useful if grid isn't built yet)
    TraceResult_t TraceLine(Vector2 start, Vector2 end, 
                         const std::vector<Entity*>& entities, 
                         Entity* ignore = nullptr) 
    {
        // Build grid for this query if using explicit entity list
        BuildSpatialGrid(entities);
        return TraceLine(start, end, ignore);
    }
};