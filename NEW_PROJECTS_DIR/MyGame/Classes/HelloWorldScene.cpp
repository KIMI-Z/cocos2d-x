/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "MapCellInfo.h"
#include "Libraries.h"
USING_NS_CC;

Scene *HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char *filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{

    log("HelloWorld::init() is called!");
    // std::cout << "HelloWorld::init() is called! by c++" << std::endl;
    // printf("HelloWorld::init() is called!");

    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setTag(101);
        closeItem->setPosition(Vec2(x, y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    menu->setTag(102);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer

        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    // auto sprite = Sprite::create("HelloWorld.png");
    // if (sprite == nullptr)
    // {
    //     problemLoading("'HelloWorld.png'");
    // }
    // else
    // {
    //     auto size = sprite->getContentSize();
    //     log("HelloWorld sprite size: %f x %f", size.width, size.height);
    //     auto visibleSize = Director::getInstance()->getVisibleSize();
    //     // position the sprite on the center of the screen
    //     sprite->setPosition(Vec2(size.width / 2 + origin.x, visibleSize.height - size.height / 2 + origin.y));
    //     auto origin = Director::getInstance()->getVisibleOrigin();
    //     log("Visible origin: %f, %f", origin.x, origin.y);

    //     // sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    //     //    sprite->setPosition(Vec2(0,0));
    //     sprite->setTag(100);
    //     // add the sprite as a child to this layer
    //     this->addChild(sprite, 0);
    //     auto spriteSize = sprite->getContentSize();
    //     log("Sprite size: %f x %f", spriteSize.width, spriteSize.height);

    //     // auto actualSize sprite->getBoundingBox().size;
    //     // log("Sprite actual size: %f x %f", actualSize.width, actualSize.height);
    // }
    // auto sprite2 = Sprite::create("HelloWorld.png");
    // if (sprite2 == nullptr)
    // {
    //     problemLoading("'HelloWorld.png'");
    // }
    // else
    // {
    //     auto size = sprite2->getContentSize();
    //     log("HelloWorld sprite size: %f x %f", size.width, size.height);
    //     auto visibleSize = Director::getInstance()->getVisibleSize();
    //     // position the sprite on the center of the screen
    //     sprite2->setPosition(Vec2(size.width + size.width / 2 + origin.x, visibleSize.height - size.height / 2 + origin.y));
    //     // sprite2->setTag(101);
    //     // add the sprite as a child to this layer
    //     this->addChild(sprite2, 0);
    // }
    // log("sprite2 position: %f, %f", sprite2->getPositionX(), sprite2->getPositionY());

    auto sprite80 = Sprite::create("80.png");
    if (sprite80 == nullptr)
    {
        problemLoading("'80.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite80->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        // add the sprite as a child to this layer
        this->addChild(sprite80, 0);
    }

    auto sprite81 = Sprite::create("81.png");
    if (sprite81 == nullptr)
    {
        problemLoading("'81.png'");
    }
    else
    { // position the sprite on the center of the screen
        auto size = sprite81->getContentSize();
        log("Sprite 81 size: %f x %f", size.width, size.height);
        sprite81->setPosition(Vec2(visibleSize.width / 2 + origin.x + size.width, visibleSize.height / 2 + origin.y));
        // add the sprite as a child to this layer
        this->addChild(sprite81, 0);
    }
    return true;
}
cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);

Door *HelloWorld::GetDoor(uint8_t Index)
{
    for (auto door : Doors)
    {
        if (door->index == Index)
        {
            return door;
        }
    }
    return nullptr;
}

void HelloWorld::menuCloseCallback(Ref *pSender)
{

    auto libraries = Libraries::getInstance();

    // 等待库加载完成
    while (!libraries->Loaded)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    log("HelloWorld::menuCloseCallback() is called!");
    auto mapReader = new Client::MapReader("0.map");
    int mapWidth = mapReader->Width;
    int mapHeight = mapReader->Height;
    log("Map size: %d x %d", mapWidth, mapHeight);
    // 访问单元格数据
    // for (int x = 0; x < mapWidth; x++)
    // {
    //     for (int y = 0; y < mapHeight; y++)
    //     {
    //         auto cell = mapReader->MapCells[x][y];
    //         // 处理瓦片数据
    //         int backImage = cell->BackImage;
    //         int middleImage = cell->MiddleImage;
    //         int frontImage = cell->FrontImage;
    //         // log("Cell (%d, %d): BackImage=%d, MiddleImage=%d, FrontImage=%d", x, y, backImage, middleImage, frontImage );

    //         // ...
    //     }
    // }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    int MovementX = 350;
    int MovementY = 350;
    int ViewRangeX = 30;
    int ViewRangeY = 30;
    int startX = MovementX - ViewRangeX;
    int endX = MovementX + ViewRangeX;
    int startY = MovementY - ViewRangeY;
    int endY = MovementY + ViewRangeY;
    int endYExtended = endY + 5;

    int Height = mapReader->Height;
    int Width = mapReader->Width;
    int OffSetX = visibleSize.width / 2 / 48;
    int OffSetY = visibleSize.height / 2 / 32;
    int CellWidth = 48;
    int CellHeight = 32;
    int AnimationCount = 0;

    // Create drawXCache vector
    std::vector<int> drawXCache(endX - startX + 1);
    for (int xi = startX; xi <= endX; xi++)
        drawXCache[xi - startX] = (xi - MovementX + OffSetX) * CellWidth - OffSetX + MovementX;

    // Create drawYCache vector
    std::vector<int> drawYCache(endYExtended - startY + 1);
    for (int yi = startY; yi <= endYExtended; yi++)
        drawYCache[yi - startY] = (yi - MovementY + OffSetY) * CellHeight + MovementY;

    // auto M2CellInfo = mapReader->MapCells;
    for (int y = startY; y <= endYExtended; y++)
    {
        if (y <= 0)
            continue;
        if (y >= Height)
            break;

        int drawY = drawYCache[y - startY];
        // log("Drawing drawY=%d", drawY);

        for (int x = startX; x <= endX; x++)
        {
            if (x < 0)
                continue;
            if (x >= Width)
                break;

            int drawX = drawXCache[x - startX];
            // log("Drawing cell at (%d, %d) with drawX=%d, drawY=%d", x, y, drawX, drawY);
            auto cell = mapReader->MapCells[x][y];

            // Back
            if (y % 2 == 0 && x % 2 == 0 && y <= endY)
            {

                if (cell->BackImage != 0 && cell->BackIndex != -1)
                {
                    int index = (cell->BackImage & 0x1FFFFFFF) - 1;
                    // Console.WriteLine($ "[{x},{y}]lib BackIndex:{cell.BackIndex}");
                    // log("Cell (%d, %d): BackImage=%d, BackIndex=%d", x, y, cell->BackImage, cell->BackIndex);
                    auto lib = libraries->MapLibs[cell->BackIndex];
                    lib->draw(index, drawX, drawY);
                }
            }

            // Middle
            int midIndex = cell->MiddleImage - 1;
            if (midIndex >= 0 && cell->MiddleIndex != -1)
            {
                // Console.WriteLine($ "[{x},{y}]lib MiddleIndex:{cell.MiddleIndex}");
                // log("Cell (%d, %d): MiddleImage=%d, MiddleIndex=%d", x, y, cell->MiddleImage, cell->MiddleIndex);
                auto lib = libraries->MapLibs[cell->MiddleIndex];
                auto s = lib->getSize(midIndex);
                // log ("s.width=%d, s.height=%d", s.width, s.height);
                if ((s.width == CellWidth && s.height == CellHeight) ||
                    (s.width == CellWidth * 2 && s.height == CellHeight * 2))
                {
                    // log("Drawing middle image at (%d, %d) with size (%d, %d)", drawX, drawY, s.width, s.height);
                    lib->draw(midIndex, drawX, drawY);
                }
            }

            // Front
            int frontIndex = (cell->FrontImage & 0x7FFF) - 1;
            if (frontIndex != -1)
            {
                int fileIndex = cell->FrontIndex;
                if (fileIndex != -1 && fileIndex != 200)
                {
                    log("Cell (%d, %d): fileIndex=%d, frontIndex=%d", x, y, fileIndex, frontIndex);

                    auto lib = libraries->MapLibs[fileIndex];
                    auto s = lib->getSize(frontIndex);

                    // doorfileIndex
                    if (cell->DoorIndex > 0)
                    {
                        auto doorInfo = GetDoor(cell->DoorIndex);
                        if (doorInfo == nullptr)
                        {
                            //    doorInfo = new Door() { index = cell->DoorIndex, DoorState = 0, ImageIndex = 0, LastTick = CMain.Time };
                            //    doorInfo = new Door() { index = cell->DoorIndex, DoorState = 0, ImageIndex = 0, LastTick = 0 };
                            auto doorInfo = new Door(cell->DoorIndex, DoorState::Closed, 0, 0, Vec2(x, y));
                            Doors.push_back(doorInfo);
                        }
                        else if (doorInfo->doorState != DoorState::Open)
                        {
                            frontIndex += (doorInfo->imageIndex + 1) * cell->DoorOffset;
                        }
                    }

                    if (frontIndex >= 0 &&
                        ((s.width == CellWidth && s.height == CellHeight) ||
                         (s.width == CellWidth * 2 && s.height == CellHeight * 2)))
                    {
                        lib->draw(frontIndex, drawX, drawY);
                    }
                }
            }

            // draw object
            int index;
            unsigned char animation;
            bool blend;
            SizeLib s;

            // Draw shanda's tile animation layer
            index = mapReader->MapCells[x][y]->TileAnimationImage;
            animation = mapReader->MapCells[x][y]->TileAnimationFrames;
            if ((index > 0) && (animation > 0))
            {
                index--;
                int animationoffset = mapReader->MapCells[x][y]->TileAnimationOffset ^ 0x2000;
                index += animationoffset * (AnimationCount % animation);
                // libraries->MapLibs[190]->drawUp(index, drawX, drawY);
            }

            // Draw mir3 middle layer
            if ((mapReader->MapCells[x][y]->MiddleIndex >= 0) && (mapReader->MapCells[x][y]->MiddleIndex != -1))
            {
                index = mapReader->MapCells[x][y]->MiddleImage - 1;
                if (index > 0)
                {
                    animation = mapReader->MapCells[x][y]->MiddleAnimationFrame;
                    blend = false;
                    if ((animation > 0) && (animation < 255))
                    {
                        if ((animation & 0x0f) > 0)
                        {
                            blend = true;
                            animation &= 0x0f;
                        }
                        if (animation > 0)
                        {
                            unsigned char animationTick = mapReader->MapCells[x][y]->MiddleAnimationTick;
                            index += (AnimationCount % (animation + (animation * animationTick))) / (1 + animationTick);

                            if (blend && (animation == 10 || animation == 8)) // diamond mines, abyss blends
                            {
                                libraries->MapLibs[mapReader->MapCells[x][y]->MiddleIndex]->drawUpBlend(index, PointLib(drawX, drawY));
                            }
                            else
                            {
                                libraries->MapLibs[mapReader->MapCells[x][y]->MiddleIndex]->drawUp(index, drawX, drawY);
                            }
                        }
                    }
                    s = libraries->MapLibs[mapReader->MapCells[x][y]->MiddleIndex]->getSize(index);
                    if ((s.width != CellWidth || s.height != CellHeight) &&
                        (s.width != (CellWidth * 2) || s.height != (CellHeight * 2)) && !blend)
                    {
                        libraries->MapLibs[mapReader->MapCells[x][y]->MiddleIndex]->drawUp(index, drawX, drawY);
                    }
                }
            }

            // Draw front layer
            index = (mapReader->MapCells[x][y]->FrontImage & 0x7FFF) - 1;

            if (index < 0)
                continue;

            int fileIndex = mapReader->MapCells[x][y]->FrontIndex;
            if (fileIndex == -1)
                continue;
            animation = mapReader->MapCells[x][y]->FrontAnimationFrame;

            if ((animation & 0x80) > 0)
            {
                blend = true;
                animation &= 0x7F;
            }
            else
                blend = false;

            if (animation > 0)
            {
                unsigned char animationTick = mapReader->MapCells[x][y]->FrontAnimationTick;
                index += (AnimationCount % (animation + (animation * animationTick))) / (1 + animationTick);
            }

            if (mapReader->MapCells[x][y]->DoorIndex > 0)
            {
                Door *DoorInfo = GetDoor(mapReader->MapCells[x][y]->DoorIndex);
                if (DoorInfo == nullptr)
                {
                    DoorInfo = new Door();
                    DoorInfo->index = mapReader->MapCells[x][y]->DoorIndex;
                    DoorInfo->doorState = DoorState::Open;
                    DoorInfo->imageIndex = 0;
                    DoorInfo->lastTick = 0;
                    Doors.push_back(DoorInfo);
                }
                else
                {
                    if (DoorInfo->doorState != DoorState::Open)
                    {
                        index += (DoorInfo->imageIndex + 1) * mapReader->MapCells[x][y]->DoorOffset;
                    }
                }
            }

            s = libraries->MapLibs[fileIndex]->getSize(index);
            if (s.width == CellWidth && s.height == CellHeight && animation == 0)
                continue;
            if ((s.width == CellWidth * 2) && (s.height == CellHeight * 2) && (animation == 0))
                continue;

            if (blend)
            {
                if (fileIndex == 14 || fileIndex == 27 || (fileIndex > 99 && fileIndex < 199))
                    libraries->MapLibs[fileIndex]->drawBlend(index, PointLib(drawX, drawY - (3 * CellHeight)),
                                                             Color::White(), true);
                else
                    libraries->MapLibs[fileIndex]->drawBlend(index, PointLib(drawX, drawY - s.height),
                                                             Color::White(), (index >= 2723 && index <= 2732));
            }
            else
            {
                if (fileIndex == 28 && libraries->MapLibs[fileIndex]->getOffset(index) != PointLib::ZERO())
                    libraries->MapLibs[fileIndex]->draw(index, PointLib(drawX, drawY - CellHeight), Color::White(), true);
                else
                    libraries->MapLibs[fileIndex]->draw(index, drawX, drawY - s.height);
            }
        }
    }

    // Close the cocos2d-x game scene and quit the application
    //  Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
