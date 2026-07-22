// MapCellInfo.h
#pragma once
#include "cocos2d.h"
#include <vector>
#include <functional>

namespace Client {
    
    // 前向声明
    class MapObject;
    
    // 对象类型枚举
    enum class ObjectType {
        Item,
        Monster,
        Spell,
        None
    };
    
    // 怪物类型枚举（简化版）
    enum class Monster {
        PalaceWallLeft,
        PalaceWall1,
        PalaceWall2,
        SSabukWall1,
        SSabukWall2,
        SSabukWall3,
        HellLord
    };
    
    // 地图对象基类
    class MapObject {
    public:
        uint32_t ObjectID = 0;
        ObjectType Race = ObjectType::None;
        bool Dead = false;
        
        virtual void Draw() = 0;
        virtual ~MapObject() = default;
    };
    
    // 怪物对象
    class MonsterObject : public MapObject {
    public:
        Monster BaseImage = Monster::PalaceWallLeft;
        virtual void Draw() override {}
    };
    
    // 单元格信息
    class CellInfo {
    public:
        short BackIndex = 0;
        int BackImage = 0;
        short MiddleIndex = 0;
        int MiddleImage = 0;
        short FrontIndex = 0;
        int FrontImage = 0;
        
        uint8_t DoorIndex = 0;
        uint8_t DoorOffset = 0;
        
        uint8_t FrontAnimationFrame = 0;
        uint8_t FrontAnimationTick = 0;
        
        uint8_t MiddleAnimationFrame = 0;
        uint8_t MiddleAnimationTick = 0;
        
        short TileAnimationImage = 0;
        short TileAnimationOffset = 0;
        uint8_t TileAnimationFrames = 0;
        
        uint8_t Light = 0;
        uint8_t Unknown = 0;
        
        std::vector<MapObject*> CellObjects;
        bool FishingCell = false;
        
        void AddObject(MapObject* ob);
        void RemoveObject(MapObject* ob);
        MapObject* FindObject(uint32_t ObjectID);
        void DrawObjects();
        void DrawDeadObjects();
        void Sort();
        
        ~CellInfo();
    };
    
    // 地图读取器
    class MapReader {
    public:
        int Width = 0;
        int Height = 0;
        std::vector<std::vector<CellInfo*>> MapCells;
        
        MapReader(const std::string& fileName);
        ~MapReader();
        
    private:
        std::string FileName;
        std::vector<uint8_t> Bytes;
        
        void Initiate();
        void LoadMapType0();
        void LoadMapType1();
        void LoadMapType2();
        void LoadMapType3();
        void LoadMapType4();
        void LoadMapType5();
        void LoadMapType6();
        void LoadMapType7();
        void LoadMapType100();
        
        // 辅助函数
        int16_t ReadInt16(int& offset);
        int32_t ReadInt32(int& offset);
        uint8_t ReadByte(int& offset);
        void SkipBytes(int& offset, int count);
        
        // 日志函数（根据你的项目调整）
        void LogError(const std::string& error);
    };
    
    // 全局设置（需要根据你的项目调整）
    struct Settings {
        static bool LogErrors;
    };
    
    // 全局日志函数（需要根据你的项目实现）
    class CMain {
    public:
        static void SaveError(const std::string& error);
    };
    
} // namespace Client