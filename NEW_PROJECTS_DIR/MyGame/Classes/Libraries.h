// Libraries.h
#pragma once

#include "cocos2d.h"
#include <string>
#include <vector>
#include <thread>
#include <regex>
#include <zlib.h>
#include <fstream>
#include <experimental/filesystem>
#include <chrono>
#include <unordered_map>

namespace fs = std::experimental::filesystem;

USING_NS_CC;

// ==================== 辅助结构体 ====================
struct PointLib
{
    int x, y;
    PointLib();
    PointLib(int _x, int _y);
    void offset(int dx, int dy);
     // == 运算符重载 - 作为成员函数
    bool operator==(const PointLib& other) const
    {
        return x == other.x && y == other.y;
    }

    // != 运算符重载 - 作为成员函数
    bool operator!=(const PointLib& other) const
    {
        return !(*this == other);
    }

    // 静态零值方法
    static PointLib ZERO()
    {
        return PointLib(0, 0);
    }
};

struct SizeLib
{
    int width, height;
    SizeLib();
    SizeLib(int w, int h);
    bool isEmpty() const;
};

struct Color
{
    unsigned char r, g, b, a;
    Color();
    Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255);
    static Color White();
    static Color Black();
    cocos2d::Color4B toCocos() const;
};

// ==================== 配置类 ====================
class Settings
{
public:
    static std::string DataPath;
    static std::string CArmourPath;
    static std::string CWeaponPath;
    static std::string CWeaponEffectPath;
    static std::string CHairPath;
    static std::string CHumEffectPath;
    static std::string AArmourPath;
    static std::string AHairPath;
    static std::string AWeaponPath;
    static std::string AHumEffectPath;
    static std::string ARArmourPath;
    static std::string ARHairPath;
    static std::string ARWeaponPath;
    static std::string ARHumEffectPath;
    static std::string MonsterPath;
    static std::string GatePath;
    static std::string FlagPath;
    static std::string SiegePath;
    static std::string NPCPath;
    static std::string MountPath;
    static std::string FishingPath;
    static std::string PetsPath;
    static std::string TransformPath;
    static std::string TransformMountsPath;
    static std::string TransformEffectPath;
    static std::string TransformWeaponEffectPath;

    static int ScreenWidth;
    static int ScreenHeight;
    static int CleanDelay;
};

// ==================== 前向声明 ====================
class MLibrary;
class MImage;

// ==================== 纹理管理器 ====================
class TextureManager
{
public:
    static TextureManager *getInstance();

    cocos2d::Texture2D *getTexture(const std::string &key);
    void addTexture(const std::string &key, cocos2d::Texture2D *texture);
    void removeTexture(const std::string &key);

private:
    TextureManager() = default;
    std::unordered_map<std::string, cocos2d::Texture2D *> _textures;
};

// ==================== MImage类 ====================
class MImage
{
public:
    short Width = 0, Height = 0, X = 0, Y = 0, ShadowX = 0, ShadowY = 0;
    unsigned char Shadow = 0;
    int Length = 0;

    bool TextureValid = false;
    cocos2d::Texture2D *Image = nullptr;

    short MaskWidth = 0, MaskHeight = 0, MaskX = 0, MaskY = 0;
    int MaskLength = 0;
    cocos2d::Texture2D *MaskImage = nullptr;
    bool HasMask = false;

    long long CleanTime = 0;
    SizeLib TrueSize;

    std::vector<unsigned char> PixelData;

    MImage();
    explicit MImage(std::ifstream &reader);
    ~MImage();

    void createTexture(std::ifstream &reader);
    void disposeTexture();
    bool visiblePixel(const PointLib &p) const;
    SizeLib getTrueSize();

private:
    long long getCurrentTime();
    std::vector<unsigned char> decompressImage(const std::vector<unsigned char> &data);
};

// ==================== MLibrary类 ====================
class MLibrary
{
public:
    static constexpr const char *Extention = ".Lib";
    static constexpr int LibVersion = 3;

    explicit MLibrary(const std::string &filename);
    ~MLibrary();

    void Initialize();
    bool checkImage(int index);
    PointLib getOffset(int index);
    SizeLib getSize(int index);
    SizeLib getTrueSize(int index);

    void draw(int index, int x, int y);
    void draw(int index, PointLib point, Color colour, bool offSet = false);
    void draw(int index, PointLib point, Color colour, bool offSet, float opacity);
    void draw(int index, cocos2d::Rect section, PointLib point, Color colour, bool offSet);
    bool visiblePixel(int index, PointLib point, bool accurate);

private:
    std::string _fileName;
    std::vector<MImage *> _images;
    std::vector<int> _indexList;
    int _count = 0;
    bool _initialized = false;
    std::ifstream _fileStream;

    long long getCurrentTime();
};

// ==================== Libraries主类 ====================
class Libraries
{
public:
    static Libraries *getInstance();

    bool Loaded = false;
    int Count = 0, Progress = 0;

    // 静态库对象
    MLibrary *ChrSel;
    MLibrary *Prguse;
    MLibrary *Prguse2;
    MLibrary *Prguse3;
    MLibrary *UI_32bit;
    MLibrary *BuffIcon;
    MLibrary *Help;
    MLibrary *MiniMap;
    MLibrary *MapLinkIcon;
    MLibrary *Title;
    MLibrary *MagIcon;
    MLibrary *MagIcon2;
    MLibrary *Magic;
    MLibrary *Magic2;
    MLibrary *Magic3;
    MLibrary *Effect;
    MLibrary *MagicC;
    MLibrary *GuildSkill;
    MLibrary *Weather;
    MLibrary *Background;
    MLibrary *Dragon;
    MLibrary *Items;
    MLibrary *StateItems;
    MLibrary *FloorItems;
    MLibrary *Items_Tooltip_32bit;
    MLibrary *Deco;

    // 动态库数组
    std::vector<MLibrary *> MapLibs;
    std::vector<MLibrary *> CArmours;
    std::vector<MLibrary *> CWeapons;
    std::vector<MLibrary *> CWeaponEffect;
    std::vector<MLibrary *> CHair;
    std::vector<MLibrary *> CHumEffect;
    std::vector<MLibrary *> AArmours;
    std::vector<MLibrary *> AWeaponsL;
    std::vector<MLibrary *> AWeaponsR;
    std::vector<MLibrary *> AHair;
    std::vector<MLibrary *> AHumEffect;
    std::vector<MLibrary *> ARArmours;
    std::vector<MLibrary *> ARWeapons;
    std::vector<MLibrary *> ARWeaponsS;
    std::vector<MLibrary *> ARHair;
    std::vector<MLibrary *> ARHumEffect;
    std::vector<MLibrary *> Monsters;
    std::vector<MLibrary *> Gates;
    std::vector<MLibrary *> Flags;
    std::vector<MLibrary *> Siege;
    std::vector<MLibrary *> Mounts;
    std::vector<MLibrary *> NPCs;
    std::vector<MLibrary *> Fishing;
    std::vector<MLibrary *> Pets;
    std::vector<MLibrary *> Transform;
    std::vector<MLibrary *> TransformMounts;
    std::vector<MLibrary *> TransformEffect;
    std::vector<MLibrary *> TransformWeaponEffect;

private:
    Libraries();
    ~Libraries();

    void initLibrary(std::vector<MLibrary *> &library, const std::string &path,
                     const std::string &toStringValue, const std::string &suffix = "");
    void initMapLibs();
    void loadLibraries();
    void loadGameLibraries();

    Libraries(const Libraries &) = delete;
    Libraries &operator=(const Libraries &) = delete;
};