// MapCellInfo.cpp
#include "MapCellInfo.h"
#include <fstream>
#include <algorithm>
#include <cstring>

namespace Client
{

    // ======================== CellInfo 实现 ========================

    CellInfo::~CellInfo()
    {
        for (auto obj : CellObjects)
        {
            delete obj;
        }
        CellObjects.clear();
    }

    void CellInfo::AddObject(MapObject *ob)
    {
        if (CellObjects.empty())
        {
            CellObjects = std::vector<MapObject *>();
        }

        CellObjects.insert(CellObjects.begin(), ob);
        Sort();
    }

    void CellInfo::RemoveObject(MapObject *ob)
    {
        if (CellObjects.empty())
            return;

        auto it = std::find(CellObjects.begin(), CellObjects.end(), ob);
        if (it != CellObjects.end())
        {
            CellObjects.erase(it);
        }

        if (CellObjects.empty())
        {
            CellObjects.clear();
        }
        else
        {
            Sort();
        }
    }

    MapObject *CellInfo::FindObject(uint32_t objectID)
    {
        auto it = std::find_if(CellObjects.begin(), CellObjects.end(),
                               [objectID](MapObject *mo)
                               {
                                   return mo && mo->ObjectID == objectID;
                               });
        return it != CellObjects.end() ? *it : nullptr;
    }

    void CellInfo::DrawObjects()
    {
        if (CellObjects.empty())
            return;

        for (size_t i = 0; i < CellObjects.size(); i++)
        {
            if (!CellObjects[i]->Dead)
            {
                CellObjects[i]->Draw();
                continue;
            }

            if (CellObjects[i]->Race == ObjectType::Monster)
            {
                auto *monster = dynamic_cast<MonsterObject *>(CellObjects[i]);
                if (monster)
                {
                    switch (monster->BaseImage)
                    {
                    case Monster::PalaceWallLeft:
                    case Monster::PalaceWall1:
                    case Monster::PalaceWall2:
                    case Monster::SSabukWall1:
                    case Monster::SSabukWall2:
                    case Monster::SSabukWall3:
                    case Monster::HellLord:
                        CellObjects[i]->Draw();
                        break;
                    default:
                        continue;
                    }
                }
            }
        }
    }

    void CellInfo::DrawDeadObjects()
    {
        if (CellObjects.empty())
            return;

        for (size_t i = 0; i < CellObjects.size(); i++)
        {
            if (!CellObjects[i]->Dead)
                continue;

            if (CellObjects[i]->Race == ObjectType::Monster)
            {
                auto *monster = dynamic_cast<MonsterObject *>(CellObjects[i]);
                if (monster)
                {
                    switch (monster->BaseImage)
                    {
                    case Monster::PalaceWallLeft:
                    case Monster::PalaceWall1:
                    case Monster::PalaceWall2:
                    case Monster::SSabukWall1:
                    case Monster::SSabukWall2:
                    case Monster::SSabukWall3:
                    case Monster::HellLord:
                        continue;
                    default:
                        break;
                    }
                }
            }

            CellObjects[i]->Draw();
        }
    }

    void CellInfo::Sort()
    {
        std::sort(CellObjects.begin(), CellObjects.end(),
                  [](MapObject *ob1, MapObject *ob2)
                  {
                      if (ob1->Race == ObjectType::Item && ob2->Race != ObjectType::Item)
                          return true;
                      if (ob2->Race == ObjectType::Item && ob1->Race != ObjectType::Item)
                          return false;
                      if (ob1->Race == ObjectType::Spell && ob2->Race != ObjectType::Spell)
                          return true;
                      if (ob2->Race == ObjectType::Spell && ob1->Race != ObjectType::Spell)
                          return false;

                      if (ob2->Dead != ob1->Dead)
                      {
                          return ob2->Dead < ob1->Dead;
                      }
                      return ob1->ObjectID < ob2->ObjectID;
                  });
    }

    // ======================== MapReader 实现 ========================

    // 静态成员初始化
    bool Settings::LogErrors = true;

    MapReader::MapReader(const std::string &fileName) : FileName(fileName)
    {
        Initiate();
    }

    MapReader::~MapReader()
    {
        for (int x = 0; x < Width; x++)
        {
            for (int y = 0; y < Height; y++)
            {
                delete MapCells[x][y];
            }
        }
        MapCells.clear();
    }

    void MapReader::Initiate()
    {
        auto fileUtils = cocos2d::FileUtils::getInstance();

        // 1. 获取文件的完整路径（考虑到资源搜索路径）
        std::string fullPath = fileUtils->fullPathForFilename(FileName);
        cocos2d::log("Attempting to load map file: %s", fullPath.c_str());
        // 检查文件是否存在
        std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
        if (!file.good())
        {
            //  cocos2d::log("Map Error: %s", error.c_str());
            cocos2d::log("Map file not found: %s", fullPath.c_str());
            // log("Map file not found: ");
            // 文件不存在，创建默认地图
            Width = 1000;
            Height = 1000;
            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
                for (int y = 0; y < Height; y++)
                {
                    MapCells[x][y] = new CellInfo();
                }
            }
            return;
        }

        // 读取文件内容
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        Bytes.resize(size);
        file.read(reinterpret_cast<char *>(Bytes.data()), size);
        file.close();

        // 判断地图格式并加载
        if (Bytes.size() > 3 && Bytes[2] == 0x43 && Bytes[3] == 0x23)
        {
            LoadMapType100();
            return;
        }

        if (Bytes.size() > 0 && Bytes[0] == 0)
        {
            LoadMapType5();
            return;
        }

        if (Bytes.size() > 14 && Bytes[0] == 0x0F && Bytes[5] == 0x53 && Bytes[14] == 0x33)
        {
            LoadMapType6();
            return;
        }

        if (Bytes.size() > 19 && Bytes[0] == 0x15 && Bytes[4] == 0x32 &&
            Bytes[6] == 0x41 && Bytes[19] == 0x31)
        {
            LoadMapType4();
            return;
        }

        if (Bytes.size() > 14 && Bytes[0] == 0x10 && Bytes[2] == 0x61 &&
            Bytes[7] == 0x31 && Bytes[14] == 0x31)
        {
            LoadMapType1();
            return;
        }

        if (Bytes.size() > 19 && (Bytes[4] == 0x0F || Bytes[4] == 0x03) &&
            Bytes[18] == 0x0D && Bytes[19] == 0x0A)
        {
            int w = Bytes[0] + (Bytes[1] << 8);
            int h = Bytes[2] + (Bytes[3] << 8);
            if (Bytes.size() > (52 + (w * h * 14)))
            {
                LoadMapType3();
                return;
            }
            else
            {
                LoadMapType2();
                return;
            }
        }

        if (Bytes.size() > 11 && Bytes[0] == 0x0D && Bytes[1] == 0x4C &&
            Bytes[7] == 0x20 && Bytes[11] == 0x6D)
        {
            LoadMapType7();
            return;
        }

        // 默认加载旧格式
        LoadMapType0();
    }

    // 辅助函数实现
    int16_t MapReader::ReadInt16(int &offset)
    {
        int16_t value = Bytes[offset] | (Bytes[offset + 1] << 8);
        offset += 2;
        return value;
    }

    int32_t MapReader::ReadInt32(int &offset)
    {
        int32_t value = Bytes[offset] | (Bytes[offset + 1] << 8) |
                        (Bytes[offset + 2] << 16) | (Bytes[offset + 3] << 24);
        offset += 4;
        return value;
    }

    uint8_t MapReader::ReadByte(int &offset)
    {
        return Bytes[offset++];
    }

    void MapReader::SkipBytes(int &offset, int count)
    {
        offset += count;
    }

    void MapReader::LogError(const std::string &error)
    {
        if (Settings::LogErrors)
        {
            CMain::SaveError(error);
        }
    }

    // 地图格式加载实现（简化版，核心逻辑保留）
    void MapReader::LoadMapType0()
    {
        try
        {
            int offset = 0;
            Width = ReadInt16(offset);
            Height = ReadInt16(offset);

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 52;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackIndex = 0;
                    cell->MiddleIndex = 1;
                    cell->BackImage = ReadInt16(offset);
                    cell->MiddleImage = ReadInt16(offset);
                    cell->FrontImage = ReadInt16(offset);
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->FrontIndex = ReadByte(offset) + 2;
                    cell->Light = ReadByte(offset);

                    if ((cell->BackImage & 0x8000) != 0)
                    {
                        cell->BackImage = (cell->BackImage & 0x7FFF) | 0x20000000;
                    }

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType1()
    {
        try
        {
            int offset = 21;
            int w = ReadInt16(offset);
            int xorVal = ReadInt16(offset);
            int h = ReadInt16(offset);
            Width = w ^ xorVal;
            Height = h ^ xorVal;

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 54;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackIndex = 0;
                    cell->BackImage = ReadInt32(offset) ^ 0xAA38AA38;
                    cell->MiddleIndex = 1;
                    cell->MiddleImage = ReadInt16(offset) ^ xorVal;
                    cell->FrontImage = ReadInt16(offset) ^ xorVal;
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->FrontIndex = ReadByte(offset) + 2;
                    cell->Light = ReadByte(offset);
                    cell->Unknown = ReadByte(offset);

                    if (cell->FrontIndex == 102)
                    {
                        cell->FrontIndex = 90;
                    }

                    if (cell->FrontIndex >= 255)
                    {
                        cell->FrontIndex = -1;
                    }

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType2()
    {
        try
        {
            int offset = 0;
            Width = ReadInt16(offset);
            Height = ReadInt16(offset);

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 52;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackImage = ReadInt16(offset);
                    cell->MiddleImage = ReadInt16(offset);
                    cell->FrontImage = ReadInt16(offset);
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->FrontIndex = ReadByte(offset) + 120;
                    cell->Light = ReadByte(offset);
                    cell->BackIndex = ReadByte(offset) + 100;
                    cell->MiddleIndex = ReadByte(offset) + 110;

                    if ((cell->BackImage & 0x8000) != 0)
                    {
                        cell->BackImage = (cell->BackImage & 0x7FFF) | 0x20000000;
                    }

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    // MapCellInfo.cpp - 补全所有缺失的实现

    void MapReader::LoadMapType3()
    {
        try
        {
            int offset = 0;
            Width = ReadInt16(offset);
            Height = ReadInt16(offset);

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 52;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackImage = ReadInt16(offset);
                    cell->MiddleImage = ReadInt16(offset);
                    cell->FrontImage = ReadInt16(offset);
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->FrontIndex = ReadByte(offset) + 120;
                    cell->Light = ReadByte(offset);
                    cell->BackIndex = ReadByte(offset) + 100;
                    cell->MiddleIndex = ReadByte(offset) + 110;
                    cell->TileAnimationImage = ReadInt16(offset);
                    SkipBytes(offset, 7);
                    cell->TileAnimationFrames = ReadByte(offset);
                    cell->TileAnimationOffset = ReadInt16(offset);
                    SkipBytes(offset, 14);

                    if ((cell->BackImage & 0x8000) != 0)
                    {
                        cell->BackImage = (cell->BackImage & 0x7FFF) | 0x20000000;
                    }

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType4()
    {
        try
        {
            int offset = 31;
            int w = ReadInt16(offset);
            int xorVal = ReadInt16(offset);
            int h = ReadInt16(offset);
            Width = w ^ xorVal;
            Height = h ^ xorVal;

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 64;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackIndex = 0;
                    cell->MiddleIndex = 1;
                    cell->BackImage = ReadInt16(offset) ^ xorVal;
                    cell->MiddleImage = ReadInt16(offset) ^ xorVal;
                    cell->FrontImage = ReadInt16(offset) ^ xorVal;
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->FrontIndex = ReadByte(offset) + 2;
                    cell->Light = ReadByte(offset);

                    if ((cell->BackImage & 0x8000) != 0)
                    {
                        cell->BackImage = (cell->BackImage & 0x7FFF) | 0x20000000;
                    }

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType5()
    {
        try
        {
            uint8_t flag = 0;
            int offset = 20;
            SkipBytes(offset, 2); // Attribute
            Width = ReadInt16(offset);
            Height = ReadInt16(offset);
            SkipBytes(offset, 8); // 跳过其他头信息

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
                for (int y = 0; y < Height; y++)
                {
                    MapCells[x][y] = new CellInfo();
                }
            }

            // 读取后层瓦片
            for (int x = 0; x < (Width / 2); x++)
            {
                for (int y = 0; y < (Height / 2); y++)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        int cx = (x * 2) + (i % 2);
                        int cy = (y * 2) + (i / 2);
                        if (cx < Width && cy < Height)
                        {
                            uint8_t idx = ReadByte(offset);
                            MapCells[cx][cy]->BackIndex = (idx != 255) ? idx + 200 : -1;
                            MapCells[cx][cy]->BackImage = ReadInt16(offset) + 1;
                        }
                        else
                        {
                            SkipBytes(offset, 3);
                        }
                    }
                }
            }

            // 读取其余数据
            offset = 28 + (3 * ((Width / 2) + (Width % 2)) * (Height / 2));
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = MapCells[x][y];
                    flag = ReadByte(offset);
                    cell->MiddleAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    if (cell->FrontAnimationFrame == 255)
                        cell->FrontAnimationFrame = 0;
                    cell->FrontAnimationFrame &= 0x8F;
                    cell->FrontIndex = (ReadByte(offset) != 255) ? Bytes[offset - 1] + 200 : -1;
                    cell->MiddleIndex = (ReadByte(offset) != 255) ? Bytes[offset - 1] + 200 : -1;
                    cell->MiddleImage = ReadInt16(offset) + 1;
                    cell->FrontImage = ReadInt16(offset) + 1;
                    if ((cell->FrontImage == 1) && (cell->FrontIndex == 200))
                    {
                        cell->FrontIndex = -1;
                    }
                    SkipBytes(offset, 3);
                    cell->Light = ReadByte(offset) & 0x0F;
                    SkipBytes(offset, 2);

                    if ((flag & 0x01) != 1)
                        cell->BackImage |= 0x20000000;
                    if ((flag & 0x02) != 2)
                        cell->FrontImage |= 0x8000;

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                    else
                    {
                        cell->Light *= 2;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType6()
    {
        try
        {
            uint8_t flag = 0;
            int offset = 16;
            Width = ReadInt16(offset);
            Height = ReadInt16(offset);

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 40;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    flag = ReadByte(offset);
                    cell->BackIndex = (ReadByte(offset) != 255) ? Bytes[offset - 1] + 300 : -1;
                    cell->MiddleIndex = (ReadByte(offset) != 255) ? Bytes[offset - 1] + 300 : -1;
                    cell->FrontIndex = (ReadByte(offset) != 255) ? Bytes[offset - 1] + 300 : -1;
                    cell->BackImage = ReadInt16(offset) + 1;
                    cell->MiddleImage = ReadInt16(offset) + 1;
                    cell->FrontImage = ReadInt16(offset) + 1;

                    if ((cell->FrontImage == 1) && (cell->FrontIndex == 200))
                    {
                        cell->FrontIndex = -1;
                    }

                    cell->MiddleAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    if (cell->FrontAnimationFrame == 255)
                        cell->FrontAnimationFrame = 0;
                    if (cell->FrontAnimationFrame > 0x0F)
                    {
                        cell->FrontAnimationFrame = cell->FrontAnimationFrame & 0x0F;
                    }

                    cell->MiddleAnimationTick = 1;
                    cell->FrontAnimationTick = 1;
                    cell->Light = ReadByte(offset) & 0x0F;
                    cell->Light *= 4;
                    SkipBytes(offset, 8);

                    if ((flag & 0x01) != 1)
                        cell->BackImage |= 0x20000000;
                    if ((flag & 0x02) != 2)
                        cell->FrontImage |= 0x8000;
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType7()
    {
        try
        {
            int offset = 21;
            Width = ReadInt16(offset);
            SkipBytes(offset, 2);
            Height = ReadInt16(offset);

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 54;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackIndex = 0;
                    cell->BackImage = ReadInt32(offset);
                    cell->MiddleIndex = 1;
                    cell->MiddleImage = ReadInt16(offset);
                    cell->FrontImage = ReadInt16(offset);
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->FrontIndex = ReadByte(offset) + 2;
                    cell->Light = ReadByte(offset);
                    cell->Unknown = ReadByte(offset);

                    if ((cell->BackImage & 0x8000) != 0)
                    {
                        cell->BackImage = (cell->BackImage & 0x7FFF) | 0x20000000;
                    }

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    void MapReader::LoadMapType100()
    {
        try
        {
            int offset = 4;
            if (Bytes[0] != 1 || Bytes[1] != 0)
                return; // 只支持版本1

            Width = ReadInt16(offset);
            Height = ReadInt16(offset);

            MapCells.resize(Width);
            for (int x = 0; x < Width; x++)
            {
                MapCells[x].resize(Height);
            }

            offset = 8;
            for (int x = 0; x < Width; x++)
            {
                for (int y = 0; y < Height; y++)
                {
                    auto *cell = new CellInfo();
                    MapCells[x][y] = cell;

                    cell->BackIndex = ReadInt16(offset);
                    cell->BackImage = ReadInt32(offset);
                    cell->MiddleIndex = ReadInt16(offset);
                    cell->MiddleImage = ReadInt16(offset);
                    cell->FrontIndex = ReadInt16(offset);
                    cell->FrontImage = ReadInt16(offset);
                    cell->DoorIndex = ReadByte(offset) & 0x7F;
                    cell->DoorOffset = ReadByte(offset);
                    cell->FrontAnimationFrame = ReadByte(offset);
                    cell->FrontAnimationTick = ReadByte(offset);
                    cell->MiddleAnimationFrame = ReadByte(offset);
                    cell->MiddleAnimationTick = ReadByte(offset);
                    cell->TileAnimationImage = ReadInt16(offset);
                    cell->TileAnimationOffset = ReadInt16(offset);
                    cell->TileAnimationFrames = ReadByte(offset);
                    cell->Light = ReadByte(offset);

                    if (cell->Light >= 100 && cell->Light <= 119)
                    {
                        cell->FishingCell = true;
                    }
                }
            }
        }
        catch (const std::exception &ex)
        {
            LogError(ex.what());
        }
    }

    // 全局日志函数实现（你需要根据项目实际情况调整）
    void CMain::SaveError(const std::string &error)
    {
        // 这里实现你的日志记录逻辑
        // 例如：CCLOG("Map Error: %s", error.c_str());
        cocos2d::log("Map Error: %s", error.c_str());
    }

} // namespace Client