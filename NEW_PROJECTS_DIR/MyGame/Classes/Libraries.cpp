// Libraries.cpp
#include "Libraries.h"
#include <fstream>
#include <algorithm>
#include <cstring>

// ==================== PointLib实现 ====================
PointLib::PointLib() : x(0), y(0)
{


}
PointLib::PointLib(int _x, int _y) : x(_x), y(_y) {}
void PointLib::offset(int dx, int dy)
{
    x += dx;
    y += dy;
}

// ==================== SizeLib实现 ====================
SizeLib::SizeLib() : width(0), height(0) {}
SizeLib::SizeLib(int w, int h) : width(w), height(h) {}
bool SizeLib::isEmpty() const { return width == 0 || height == 0; }

// ==================== Color实现 ====================
Color::Color() : r(255), g(255), b(255), a(255) {}
Color::Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
    : r(_r), g(_g), b(_b), a(_a) {}
Color Color::White() { return Color(255, 255, 255); }
Color Color::Black() { return Color(0, 0, 0); }
cocos2d::Color4B Color::toCocos() const { return cocos2d::Color4B(r, g, b, a); }

// ==================== Settings静态成员定义 ====================
std::string Settings::DataPath = "Data/";
std::string Settings::CArmourPath = Settings::DataPath + "CArmour/";
std::string Settings::CWeaponPath = Settings::DataPath + "CWeapon/";
std::string Settings::CWeaponEffectPath = Settings::DataPath + "CWeaponEffect/";
std::string Settings::CHairPath = Settings::DataPath + "CHair/";
std::string Settings::CHumEffectPath = Settings::DataPath + "CHumEffect/";
std::string Settings::AArmourPath = Settings::DataPath + "AArmour/";
std::string Settings::AHairPath = Settings::DataPath + "AHair/";
std::string Settings::AWeaponPath = Settings::DataPath + "AWeapon/";
std::string Settings::AHumEffectPath = Settings::DataPath + "AHumEffect/";
std::string Settings::ARArmourPath = Settings::DataPath + "ARArmour/";
std::string Settings::ARHairPath = Settings::DataPath + "ARHair/";
std::string Settings::ARWeaponPath = Settings::DataPath + "ARWeapon/";
std::string Settings::ARHumEffectPath = Settings::DataPath + "ARHumEffect/";
std::string Settings::MonsterPath = Settings::DataPath + "Monster/";
std::string Settings::GatePath = Settings::DataPath + "Gate/";
std::string Settings::FlagPath = Settings::DataPath + "Flag/";
std::string Settings::SiegePath = Settings::DataPath + "Siege/";
std::string Settings::NPCPath = Settings::DataPath + "NPC/";
std::string Settings::MountPath = Settings::DataPath + "Mount/";
std::string Settings::FishingPath = Settings::DataPath + "Fishing/";
std::string Settings::PetsPath = Settings::DataPath + "Pets/";
std::string Settings::TransformPath = Settings::DataPath + "Transform/";
std::string Settings::TransformMountsPath = Settings::DataPath + "TransformMounts/";
std::string Settings::TransformEffectPath = Settings::DataPath + "TransformEffect/";
std::string Settings::TransformWeaponEffectPath = Settings::DataPath + "TransformWeaponEffect/";
int Settings::ScreenWidth = 1024;
int Settings::ScreenHeight = 768;
int Settings::CleanDelay = 5000 * 10;

// ==================== TextureManager实现 ====================
TextureManager *TextureManager::getInstance()
{
    static TextureManager instance;
    return &instance;
}

cocos2d::Texture2D *TextureManager::getTexture(const std::string &key)
{
    auto it = _textures.find(key);
    if (it != _textures.end())
    {
        return it->second;
    }
    return nullptr;
}

void TextureManager::addTexture(const std::string &key, cocos2d::Texture2D *texture)
{
    _textures[key] = texture;
}

void TextureManager::removeTexture(const std::string &key)
{
    auto it = _textures.find(key);
    if (it != _textures.end())
    {
        it->second->release();
        _textures.erase(it);
    }
}

// ==================== MImage实现 ====================
MImage::MImage() {}

MImage::MImage(std::ifstream &reader)
{
    reader.read(reinterpret_cast<char *>(&Width), 2);
    reader.read(reinterpret_cast<char *>(&Height), 2);
    reader.read(reinterpret_cast<char *>(&X), 2);
    reader.read(reinterpret_cast<char *>(&Y), 2);
    reader.read(reinterpret_cast<char *>(&ShadowX), 2);
    reader.read(reinterpret_cast<char *>(&ShadowY), 2);
    reader.read(reinterpret_cast<char *>(&Shadow), 1);
    reader.read(reinterpret_cast<char *>(&Length), 4);

    HasMask = ((Shadow >> 7) == 1);
    if (HasMask)
    {
        reader.seekg(Length, std::ios::cur);
        reader.read(reinterpret_cast<char *>(&MaskWidth), 2);
        reader.read(reinterpret_cast<char *>(&MaskHeight), 2);
        reader.read(reinterpret_cast<char *>(&MaskX), 2);
        reader.read(reinterpret_cast<char *>(&MaskY), 2);
        reader.read(reinterpret_cast<char *>(&MaskLength), 4);
    }
}

MImage::~MImage()
{
    disposeTexture();
}

void MImage::createTexture(std::ifstream &reader)
{
    std::vector<unsigned char> compressedData(Length);
    reader.read(reinterpret_cast<char *>(compressedData.data()), Length);

    std::vector<unsigned char> decompressed = decompressImage(compressedData);
    PixelData = decompressed;

    auto texture = new cocos2d::Texture2D();
    std::string key = std::to_string(reinterpret_cast<long long>(this));

    Image = texture;

    cocos2d::Image *cocosImage = new cocos2d::Image();
    cocosImage->initWithRawData(decompressed.data(), decompressed.size(), Width, Height, 8);

    texture->initWithImage(cocosImage);
    cocosImage->release();

    TextureManager::getInstance()->addTexture(key, texture);

    if (HasMask)
    {
        std::vector<unsigned char> maskCompressed(MaskLength);
        reader.read(reinterpret_cast<char *>(maskCompressed.data()), MaskLength);
        std::vector<unsigned char> maskDecompressed = decompressImage(maskCompressed);

        cocos2d::Image *maskCocosImage = new cocos2d::Image();
        maskCocosImage->initWithRawData(maskDecompressed.data(), maskDecompressed.size(), Width, Height, 8);

        MaskImage = new cocos2d::Texture2D();
        MaskImage->initWithImage(maskCocosImage);
        maskCocosImage->release();

        TextureManager::getInstance()->addTexture(key + "_mask", MaskImage);
    }

    TextureValid = true;
    CleanTime = getCurrentTime() + Settings::CleanDelay;
}

void MImage::disposeTexture()
{
    if (Image)
    {
        std::string key = std::to_string(reinterpret_cast<long long>(this));
        TextureManager::getInstance()->removeTexture(key);
        Image->release();
        Image = nullptr;
    }

    if (MaskImage)
    {
        std::string key = std::to_string(reinterpret_cast<long long>(this)) + "_mask";
        TextureManager::getInstance()->removeTexture(key);
        MaskImage->release();
        MaskImage = nullptr;
    }

    TextureValid = false;
    PixelData.clear();
}

bool MImage::visiblePixel(const PointLib &p) const
{
    if (p.x < 0 || p.y < 0 || p.x >= Width || p.y >= Height)
        return false;

    int w = Width;
    int index = (p.y * w * 4) + (p.x * 4) + 3;

    if (index >= static_cast<int>(PixelData.size()))
        return false;

    return PixelData[index] != 0;
}

SizeLib MImage::getTrueSize()
{
    if (!TrueSize.isEmpty())
        return TrueSize;

    int l = 0, t = 0, r = Width, b = Height;

    bool visible = false;
    for (int x = 0; x < r; x++)
    {
        for (int y = 0; y < b; y++)
        {
            if (visiblePixel(PointLib(x, y)))
            {
                visible = true;
                break;
            }
        }
        if (visible)
        {
            l = x;
            break;
        }
    }

    visible = false;
    for (int y = 0; y < b; y++)
    {
        for (int x = l; x < r; x++)
        {
            if (visiblePixel(PointLib(x, y)))
            {
                visible = true;
                break;
            }
        }
        if (visible)
        {
            t = y;
            break;
        }
    }

    visible = false;
    for (int x = r - 1; x >= l; x--)
    {
        for (int y = 0; y < b; y++)
        {
            if (visiblePixel(PointLib(x, y)))
            {
                visible = true;
                break;
            }
        }
        if (visible)
        {
            r = x + 1;
            break;
        }
    }

    visible = false;
    for (int y = b - 1; y >= t; y--)
    {
        for (int x = l; x < r; x++)
        {
            if (visiblePixel(PointLib(x, y)))
            {
                visible = true;
                break;
            }
        }
        if (visible)
        {
            b = y + 1;
            break;
        }
    }

    TrueSize = SizeLib(r - l, b - t);
    return TrueSize;
}

long long MImage::getCurrentTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

std::vector<unsigned char> MImage::decompressImage(const std::vector<unsigned char> &data)
{
    std::vector<unsigned char> result;

    z_stream zs = {};
    inflateInit2(&zs, 16 + MAX_WBITS);

    zs.avail_in = data.size();
    zs.next_in = const_cast<Bytef *>(data.data());

    unsigned char outbuf[32768];
    do
    {
        zs.avail_out = sizeof(outbuf);
        zs.next_out = outbuf;
        int ret = inflate(&zs, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
        {
            inflateEnd(&zs);
            return result;
        }
        result.insert(result.end(), outbuf, outbuf + sizeof(outbuf) - zs.avail_out);
    } while (zs.avail_out == 0);

    inflateEnd(&zs);
    return result;
}

// ==================== MLibrary实现 ====================
MLibrary::MLibrary(const std::string &filename)
    : _fileName(filename + Extention)
{
    auto fileUtils = cocos2d::FileUtils::getInstance();
    // 1. 获取文件的完整路径（考虑到资源搜索路径）
    std::string fullPath = fileUtils->fullPathForFilename(_fileName);
    // log("Attempting to load library file: %s", fullPath.c_str());
    if (fullPath.empty() || !fs::exists(fullPath))
    {
        //  log("Library file not found: %s", fullPath.c_str());
        return;
    }
    else
    {
        _fileName = fullPath;
    }
}

MLibrary::~MLibrary()
{
    if (_fileStream.is_open())
    {
        _fileStream.close();
    }

    for (auto image : _images)
    {
        delete image;
    }
    _images.clear();
}

void MLibrary::Initialize()
{
    _initialized = true;

    if (!fs::exists(_fileName))
    {
        // log("Library file not found: %s", _fileName.c_str());
        return;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Settings::ScreenWidth = static_cast<int>(visibleSize.width);
    Settings::ScreenHeight = static_cast<int>(visibleSize.height);

    try
    {
        _fileStream.open(_fileName, std::ios::binary);
        if (!_fileStream.is_open())
            return;

        int currentVersion;
        _fileStream.read(reinterpret_cast<char *>(&currentVersion), 4);

        if (currentVersion < 2)
        {
            cocos2d::log("Wrong version, expecting lib version: %d found version: %d", LibVersion, currentVersion);
            return;
        }

        _fileStream.read(reinterpret_cast<char *>(&_count), 4);

        int frameSeek = 0;
        if (currentVersion >= 3)
        {
            _fileStream.read(reinterpret_cast<char *>(&frameSeek), 4);
        }
        _images.resize(_count, nullptr);
        _indexList.resize(_count);
        // log("Library file not found: %s, count: %d", _fileName.c_str(), _count);

        for (int i = 0; i < _count; i++)
        {
            _fileStream.read(reinterpret_cast<char *>(&_indexList[i]), 4);
        }

        if (currentVersion >= 3 && frameSeek > 0)
        {
            _fileStream.seekg(frameSeek, std::ios::beg);
            int frameCount;
            _fileStream.read(reinterpret_cast<char *>(&frameCount), 4);
        }
    }
    catch (const std::exception &e)
    {
        cocos2d::log("Error loading library: %s", e.what());
        _initialized = false;
        throw;
    }
}

bool MLibrary::checkImage(int index)
{
    // log("checkImage (%d)", index);
    if (!_initialized)
        Initialize();

    if (index < 0 || index >= static_cast<int>(_images.size()))
        return false;

    if (!_images[index])
    {
        _fileStream.seekg(_indexList[index], std::ios::beg);
        _images[index] = new MImage(_fileStream);

        MImage *mi = _images[index];
        if (mi->Width > 0 && mi->Height > 0)
        {
            _fileStream.seekg(_indexList[index] + 17, std::ios::beg);
            mi->createTexture(_fileStream);
        }
    }

    return _images[index]->TextureValid;
}

PointLib MLibrary::getOffset(int index)
{
    if (!checkImage(index))
    {

        return PointLib();
    }

    return PointLib(_images[index]->X, _images[index]->Y);
}

SizeLib MLibrary::getSize(int index)
{
    if (!checkImage(index))
        return SizeLib();
    return SizeLib(_images[index]->Width, _images[index]->Height);
}

SizeLib MLibrary::getTrueSize(int index)
{
    if (!checkImage(index))
        return SizeLib();
    return _images[index]->getTrueSize();
}

void MLibrary::draw(int index, int x, int y)
{
    draw(index, PointLib(x, y), Color::White(), false);
}

void MLibrary::draw(int index, PointLib point, Color colour, bool offSet)
{
    if (!checkImage(index))
    {
        log("checkImage failed (%d)", index);
        return;
    }

    log("checkImage success (%d)", index);
    MImage *mi = _images[index];
    if (offSet)
        point.offset(mi->X, mi->Y);

    if (point.x >= Settings::ScreenWidth || point.y >= Settings::ScreenHeight ||
        point.x + mi->Width < 0 || point.y + mi->Height < 0)
        return;

    if (mi->Image)
    {
        auto sprite = cocos2d::Sprite::createWithTexture(mi->Image);
        sprite->setPosition(point.x + mi->Width / 2, Settings::ScreenHeight - (point.y + mi->Height / 2));
        sprite->setColor(cocos2d::Color3B(colour.r, colour.g, colour.b));
        sprite->setOpacity(colour.a);

        auto scene = cocos2d::Director::getInstance()->getRunningScene();
        if (scene)
        {
            scene->addChild(sprite);
            sprite->runAction(cocos2d::Sequence::create(
                cocos2d::DelayTime::create(Settings::CleanDelay / 1000.0f),
                cocos2d::RemoveSelf::create(),
                nullptr));
        }
    }

    mi->CleanTime = getCurrentTime() + Settings::CleanDelay;
}

void MLibrary::draw(int index, PointLib point, Color colour, bool offSet, float opacity)
{
    Color newColor = colour;
    newColor.a = static_cast<unsigned char>(opacity * 255);
    draw(index, point, newColor, offSet);
}

void MLibrary::draw(int index, cocos2d::Rect section, PointLib point, Color colour, bool offSet)
{
    if (!checkImage(index))
        return;

    MImage *mi = _images[index];
    if (offSet)
        point.offset(mi->X, mi->Y);

    if (mi->Image)
    {
        auto sprite = cocos2d::Sprite::createWithTexture(mi->Image);
        sprite->setTextureRect(section);
        sprite->setPosition(point.x + section.size.width / 2,
                            Settings::ScreenHeight - (point.y + section.size.height / 2));
        sprite->setColor(cocos2d::Color3B(colour.r, colour.g, colour.b));
        sprite->setOpacity(colour.a);

        auto scene = cocos2d::Director::getInstance()->getRunningScene();
        if (scene)
        {
            scene->addChild(sprite);
            sprite->runAction(cocos2d::Sequence::create(
                cocos2d::DelayTime::create(Settings::CleanDelay / 1000.0f),
                cocos2d::RemoveSelf::create(),
                nullptr));
        }
    }

    mi->CleanTime = getCurrentTime() + Settings::CleanDelay;
}

bool MLibrary::visiblePixel(int index, PointLib point, bool accurate)
{
    if (!checkImage(index))
        return false;

    if (accurate)
    {
        return _images[index]->visiblePixel(point);
    }

    int accuracy = 2;
    for (int x = -accuracy; x <= accuracy; x++)
    {
        for (int y = -accuracy; y <= accuracy; y++)
        {
            if (_images[index]->visiblePixel(PointLib(point.x + x, point.y + y)))
                return true;
        }
    }
    return false;
}

long long MLibrary::getCurrentTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

// ==================== Libraries实现 ====================
Libraries *Libraries::getInstance()
{
    static Libraries instance;
    return &instance;
}

Libraries::Libraries()
{

    ChrSel = new MLibrary(Settings::DataPath + "ChrSel");
    Prguse = new MLibrary(Settings::DataPath + "Prguse");
    Prguse2 = new MLibrary(Settings::DataPath + "Prguse2");
    Prguse3 = new MLibrary(Settings::DataPath + "Prguse3");
    UI_32bit = new MLibrary(Settings::DataPath + "UI_32bit");
    Title = new MLibrary(Settings::DataPath + "Title");
    Background = new MLibrary(Settings::DataPath + "Background");
    Dragon = new MLibrary(Settings::DataPath + "Dragon");
    BuffIcon = new MLibrary(Settings::DataPath + "BuffIcon");
    Help = new MLibrary(Settings::DataPath + "Help");
    MiniMap = new MLibrary(Settings::DataPath + "MMap");
    MapLinkIcon = new MLibrary(Settings::DataPath + "MapLinkIcon");
    MagIcon = new MLibrary(Settings::DataPath + "MagIcon");
    MagIcon2 = new MLibrary(Settings::DataPath + "MagIcon2");
    Magic = new MLibrary(Settings::DataPath + "Magic");
    Magic2 = new MLibrary(Settings::DataPath + "Magic2");
    Magic3 = new MLibrary(Settings::DataPath + "Magic3");
    Effect = new MLibrary(Settings::DataPath + "Effect");
    MagicC = new MLibrary(Settings::DataPath + "MagicC");
    GuildSkill = new MLibrary(Settings::DataPath + "GuildSkill");
    Weather = new MLibrary(Settings::DataPath + "Weather");
    Items = new MLibrary(Settings::DataPath + "Items");
    StateItems = new MLibrary(Settings::DataPath + "StateItem");
    FloorItems = new MLibrary(Settings::DataPath + "DNItems");
    Items_Tooltip_32bit = new MLibrary(Settings::DataPath + "Items_Tooltip_32bit");
    Deco = new MLibrary(Settings::DataPath + "Deco");

    initLibrary(CArmours, Settings::CArmourPath, "00");
    initLibrary(CHair, Settings::CHairPath, "00");
    initLibrary(CWeapons, Settings::CWeaponPath, "00");
    initLibrary(CWeaponEffect, Settings::CWeaponEffectPath, "00");
    initLibrary(CHumEffect, Settings::CHumEffectPath, "00");

    initLibrary(AArmours, Settings::AArmourPath, "00");
    initLibrary(AHair, Settings::AHairPath, "00");
    initLibrary(AWeaponsL, Settings::AWeaponPath, "00", " L");
    initLibrary(AWeaponsR, Settings::AWeaponPath, "00", " R");
    initLibrary(AHumEffect, Settings::AHumEffectPath, "00");

    initLibrary(ARArmours, Settings::ARArmourPath, "00");
    initLibrary(ARHair, Settings::ARHairPath, "00");
    initLibrary(ARWeapons, Settings::ARWeaponPath, "00");
    initLibrary(ARWeaponsS, Settings::ARWeaponPath, "00", " S");
    initLibrary(ARHumEffect, Settings::ARHumEffectPath, "00");

    initLibrary(Monsters, Settings::MonsterPath, "000");
    initLibrary(Gates, Settings::GatePath, "00");
    initLibrary(Flags, Settings::FlagPath, "00");
    initLibrary(Siege, Settings::SiegePath, "00");
    initLibrary(NPCs, Settings::NPCPath, "00");
    initLibrary(Mounts, Settings::MountPath, "00");
    initLibrary(Fishing, Settings::FishingPath, "00");
    initLibrary(Pets, Settings::PetsPath, "00");
    initLibrary(Transform, Settings::TransformPath, "00");
    initLibrary(TransformMounts, Settings::TransformMountsPath, "00");
    initLibrary(TransformEffect, Settings::TransformEffectPath, "00");
    initLibrary(TransformWeaponEffect, Settings::TransformWeaponEffectPath, "00");

    initMapLibs();
    loadLibraries();

    std::thread([this]()
                { loadGameLibraries(); })
        .detach();
}

Libraries::~Libraries()
{
    delete ChrSel;
    delete Prguse;
    delete Prguse2;
    delete Prguse3;
    delete UI_32bit;
    delete Title;
    delete Background;
    delete Dragon;
    delete BuffIcon;
    delete Help;
    delete MiniMap;
    delete MapLinkIcon;
    delete MagIcon;
    delete MagIcon2;
    delete Magic;
    delete Magic2;
    delete Magic3;
    delete Effect;
    delete MagicC;
    delete GuildSkill;
    delete Weather;
    delete Items;
    delete StateItems;
    delete FloorItems;
    delete Items_Tooltip_32bit;
    delete Deco;

    for (auto lib : MapLibs)
        delete lib;
    for (auto lib : CArmours)
        delete lib;
    for (auto lib : CWeapons)
        delete lib;
    for (auto lib : CWeaponEffect)
        delete lib;
    for (auto lib : CHair)
        delete lib;
    for (auto lib : CHumEffect)
        delete lib;
    for (auto lib : AArmours)
        delete lib;
    for (auto lib : AWeaponsL)
        delete lib;
    for (auto lib : AWeaponsR)
        delete lib;
    for (auto lib : AHair)
        delete lib;
    for (auto lib : AHumEffect)
        delete lib;
    for (auto lib : ARArmours)
        delete lib;
    for (auto lib : ARWeapons)
        delete lib;
    for (auto lib : ARWeaponsS)
        delete lib;
    for (auto lib : ARHair)
        delete lib;
    for (auto lib : ARHumEffect)
        delete lib;
    for (auto lib : Monsters)
        delete lib;
    for (auto lib : Gates)
        delete lib;
    for (auto lib : Flags)
        delete lib;
    for (auto lib : Siege)
        delete lib;
    for (auto lib : Mounts)
        delete lib;
    for (auto lib : NPCs)
        delete lib;
    for (auto lib : Fishing)
        delete lib;
    for (auto lib : Pets)
        delete lib;
    for (auto lib : Transform)
        delete lib;
    for (auto lib : TransformMounts)
        delete lib;
    for (auto lib : TransformEffect)
        delete lib;
    for (auto lib : TransformWeaponEffect)
        delete lib;
}

void Libraries::initLibrary(std::vector<MLibrary *> &library, const std::string &path,
                            const std::string &toStringValue, const std::string &suffix)
{
    if (!fs::exists(path))
    {
        fs::create_directories(path);
    }

    std::vector<int> indices;
    for (const auto &entry : fs::directory_iterator(path))
    {
        std::string filename = entry.path().filename().string();
        if (filename.find(suffix + MLibrary::Extention) != std::string::npos)
        {
            std::regex re("\\d+");
            std::smatch match;
            if (std::regex_search(filename, match, re))
            {
                indices.push_back(std::stoi(match.str()));
            }
        }
    }

    std::sort(indices.begin(), indices.end());
    int count = indices.empty() ? 0 : indices.back() + 1;

    library.resize(count, nullptr);
    for (int i = 0; i < count; i++)
    {
        std::string numStr = std::to_string(i);
        if (toStringValue.length() > numStr.length())
        {
            numStr = std::string(toStringValue.length() - numStr.length(), '0') + numStr;
        }
        library[i] = new MLibrary(path + numStr + suffix);
    }
}

void Libraries::initMapLibs()
{
    MapLibs.resize(400, nullptr);

    MapLibs[0] = new MLibrary(Settings::DataPath + "Map/WemadeMir2/Tiles");
    MapLibs[1] = new MLibrary(Settings::DataPath + "Map/WemadeMir2/Smtiles");
    MapLibs[2] = new MLibrary(Settings::DataPath + "Map/WemadeMir2/Objects");
    for (int i = 2; i < 28; i++)
    {
        MapLibs[i + 1] = new MLibrary(Settings::DataPath + "Map/WemadeMir2/Objects" + std::to_string(i));
    }
    MapLibs[90] = new MLibrary(Settings::DataPath + "Map/WemadeMir2/Objects_32bit");

    MapLibs[100] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/Tiles");
    for (int i = 1; i < 10; i++)
    {
        MapLibs[100 + i] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/Tiles" + std::to_string(i + 1));
    }
    MapLibs[110] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/SmTiles");
    for (int i = 1; i < 10; i++)
    {
        MapLibs[110 + i] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/SmTiles" + std::to_string(i + 1));
    }
    MapLibs[120] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/Objects");
    for (int i = 1; i < 31; i++)
    {
        MapLibs[120 + i] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/Objects" + std::to_string(i + 1));
    }
    MapLibs[190] = new MLibrary(Settings::DataPath + "Map/ShandaMir2/AniTiles1");

    std::vector<std::string> Mapstate = {"", "wood/", "sand/", "snow/", "forest/"};
    for (size_t i = 0; i < Mapstate.size(); i++)
    {
        int baseIdx = 200 + i * 15;
        MapLibs[baseIdx] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Tilesc");
        MapLibs[baseIdx + 1] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Tiles30c");
        MapLibs[baseIdx + 2] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Tiles5c");
        MapLibs[baseIdx + 3] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Smtilesc");
        MapLibs[baseIdx + 4] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Housesc");
        MapLibs[baseIdx + 5] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Cliffsc");
        MapLibs[baseIdx + 6] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Dungeonsc");
        MapLibs[baseIdx + 7] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Innersc");
        MapLibs[baseIdx + 8] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Furnituresc");
        MapLibs[baseIdx + 9] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Wallsc");
        MapLibs[baseIdx + 10] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "smObjectsc");
        MapLibs[baseIdx + 11] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Animationsc");
        MapLibs[baseIdx + 12] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Object1c");
        MapLibs[baseIdx + 13] = new MLibrary(Settings::DataPath + "Map/WemadeMir3/" + Mapstate[i] + "Object2c");
    }

    Mapstate = {"", "wood", "sand", "snow", "forest"};
    for (size_t i = 0; i < Mapstate.size(); i++)
    {
        int baseIdx = 300 + i * 15;
        MapLibs[baseIdx] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Tilesc" + Mapstate[i]);
        MapLibs[baseIdx + 1] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Tiles30c" + Mapstate[i]);
        MapLibs[baseIdx + 2] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Tiles5c" + Mapstate[i]);
        MapLibs[baseIdx + 3] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Smtilesc" + Mapstate[i]);
        MapLibs[baseIdx + 4] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Housesc" + Mapstate[i]);
        MapLibs[baseIdx + 5] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Cliffsc" + Mapstate[i]);
        MapLibs[baseIdx + 6] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Dungeonsc" + Mapstate[i]);
        MapLibs[baseIdx + 7] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Innersc" + Mapstate[i]);
        MapLibs[baseIdx + 8] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Furnituresc" + Mapstate[i]);
        MapLibs[baseIdx + 9] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Wallsc" + Mapstate[i]);
        MapLibs[baseIdx + 10] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/smObjectsc" + Mapstate[i]);
        MapLibs[baseIdx + 11] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Animationsc" + Mapstate[i]);
        MapLibs[baseIdx + 12] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Object1c" + Mapstate[i]);
        MapLibs[baseIdx + 13] = new MLibrary(Settings::DataPath + "Map/ShandaMir3/Object2c" + Mapstate[i]);
    }
}

void Libraries::loadLibraries()
{
    ChrSel->Initialize();
    Progress++;
    Prguse->Initialize();
    Progress++;
    Prguse2->Initialize();
    Progress++;
    Prguse3->Initialize();
    Progress++;
    UI_32bit->Initialize();
    Progress++;
    Title->Initialize();
    Progress++;
}

void Libraries::loadGameLibraries()
{
    Count = static_cast<int>(MapLibs.size() + Monsters.size() + Gates.size() + Flags.size() +
                             Siege.size() + NPCs.size() + CArmours.size() + CHair.size() +
                             CWeapons.size() + CWeaponEffect.size() + AArmours.size() +
                             AHair.size() + AWeaponsL.size() + AWeaponsR.size() + ARArmours.size() +
                             ARHair.size() + ARWeapons.size() + ARWeaponsS.size() + CHumEffect.size() +
                             AHumEffect.size() + ARHumEffect.size() + Mounts.size() + Fishing.size() +
                             Pets.size() + Transform.size() + TransformMounts.size() +
                             TransformEffect.size() + TransformWeaponEffect.size() + 19);

    Dragon->Initialize();
    Progress++;
    BuffIcon->Initialize();
    Progress++;
    Help->Initialize();
    Progress++;
    MiniMap->Initialize();
    Progress++;
    MapLinkIcon->Initialize();
    Progress++;
    MagIcon->Initialize();
    Progress++;
    MagIcon2->Initialize();
    Progress++;
    Magic->Initialize();
    Progress++;
    Magic2->Initialize();
    Progress++;
    Magic3->Initialize();
    Progress++;
    MagicC->Initialize();
    Progress++;
    Effect->Initialize();
    Progress++;
    Weather->Initialize();
    Progress++;
    GuildSkill->Initialize();
    Progress++;
    Background->Initialize();
    Progress++;
    Deco->Initialize();
    Progress++;
    Items->Initialize();
    Progress++;
    StateItems->Initialize();
    Progress++;
    FloorItems->Initialize();
    Progress++;
    Items_Tooltip_32bit->Initialize();
    Progress++;

    for (auto lib : MapLibs)
    {
        if (lib)
            lib->Initialize();
        Progress++;
    }

    auto loadLibArray = [this](std::vector<MLibrary *> &libs)
    {
        for (auto lib : libs)
        {
            if (lib)
                lib->Initialize();
            Progress++;
        }
    };

    loadLibArray(Monsters);
    loadLibArray(Gates);
    loadLibArray(Flags);
    loadLibArray(Siege);
    loadLibArray(NPCs);
    loadLibArray(CArmours);
    loadLibArray(CHair);
    loadLibArray(CWeapons);
    loadLibArray(CWeaponEffect);
    loadLibArray(AArmours);
    loadLibArray(AHair);
    loadLibArray(AWeaponsL);
    loadLibArray(AWeaponsR);
    loadLibArray(ARArmours);
    loadLibArray(ARHair);
    loadLibArray(ARWeapons);
    loadLibArray(ARWeaponsS);
    loadLibArray(CHumEffect);
    loadLibArray(AHumEffect);
    loadLibArray(ARHumEffect);
    loadLibArray(Mounts);
    loadLibArray(Fishing);
    loadLibArray(Pets);
    loadLibArray(Transform);
    loadLibArray(TransformEffect);
    loadLibArray(TransformWeaponEffect);
    loadLibArray(TransformMounts);

    Loaded = true;
    cocos2d::log("All libraries loaded!");
}