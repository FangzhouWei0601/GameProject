// include/headers/map/MapTypes.h
#pragma once
#include <string>
#include <glm/glm.hpp>

enum class AreaType {
    Normal,         
    Safe,          
    Battle,        
    Boss,          
    Secret         
};

struct AreaBounds {
    glm::vec2 position;  
    glm::vec2 size;      

    bool containsPoint(const glm::vec2& point) const {
        return point.x >= position.x && point.x < position.x + size.x &&
            point.y >= position.y && point.y < position.y + size.y;
    }
};

struct PortalData {
    std::string targetAreaId;  
    glm::vec2 position;        
    glm::vec2 size;           
    glm::vec2 targetPosition; 
    bool isLocked;           

    PortalData() : isLocked(false) {}
};

struct AreaData {
    std::string id;           
    std::string name;         
    AreaType type;           
    AreaBounds bounds;       
    bool isDiscovered;       
    bool isUnlocked;        

    AreaData() : type(AreaType::Normal), isDiscovered(false), isUnlocked(true) {}
};