#ifndef MODULE_LOG_H
#define MODULE_LOG_H

#include "../../lib/DMLog.h"
#include "module.h"

class Module_log: public Module {
public:
    virtual void import(DMJSON * import);
};

/*
 *
 * 技术的发展都是在围绕着解决“软件的复杂度”这个基本的需求而发展的。
 *
 * 这些所谓天才，看不到简单直观的解决方案，为了显示自己的聪明而采用繁复的抽象，其实是一种愚蠢。真正的天才，必须能够让事情变得简单。
 *
 * Java 却拿掉了一些人们想要的东西，且没有真正的提供一个替代品。
 *
 * 技术的细节只是被隐藏了，而不是被掩盖了，当你打开了“显示隐藏文件”的选项时，还是可以被显示出来，并且完全可以被操作。
 *
 * primitive elements. （基本元素）
 * means of combination. （组合手段）
 * means of abstraction. (抽象手段）
*/
void function_log(MemorySpace * object);


#endif /* MODULE_LOG_H */

