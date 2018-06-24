APP_MODULES := libBullet
APP_ABI := mips mips64 armeabi
APP_OPTIM := release

#We only need STL for placement new (#include <new>) 
#We don't use STL in Bullet
APP_STL                 := c++_shared
