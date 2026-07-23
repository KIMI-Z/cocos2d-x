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

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <cstdint>  // for uint8_t, int64_t
#include <string>   // if Point uses std::string or other types
#include <vector>

USING_NS_CC;  // 加上这行
// Assuming DoorState is an enum or class defined elsewhere
enum class DoorState {
    // Define your door states here
    Open,
    Closed,
    Locked,
    // etc.
};

// Assuming Point is defined elsewhere (could be a struct with x, y coordinates)
struct Point {
    int x;
    int y;
    // or use whatever coordinate type you need
};

class Door {
public:
    uint8_t index;        // byte -> uint8_t
    DoorState doorState;  // DoorState enum/class
    uint8_t imageIndex;   // byte -> uint8_t
    int64_t lastTick;     // long -> int64_t (64-bit integer)
    Vec2 location;       // Point struct/class

     // 构造函数
    Door() : index(0), doorState(DoorState::Closed), 
             imageIndex(0), lastTick(0), location(Vec2::ZERO) {}
    
    Door(uint8_t idx, DoorState state, uint8_t imgIdx, 
         long long tick, const Vec2& loc)
        : index(idx), doorState(state), imageIndex(imgIdx),
          lastTick(tick), location(loc) {}
};

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    std::vector<Door*> Doors;
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    Door* GetDoor(uint8_t Index);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};




#endif // __HELLOWORLD_SCENE_H__
