#include "sound_handler.h"

sound_handler::sound_handler(const char *ch, int roleCode, int index, int volume)
    : QString(ch)
{
    tempSoundPath.clear();
    soundRole = roleCode;
    indexPosition = index;
    volumeLevel = volume;
}

sound_handler::sound_handler(const QString &s, int roleCode, int index, int volume)
    : QString(s)
{
    tempSoundPath.clear();
    soundRole = roleCode;
    indexPosition = index;
    volumeLevel = volume;
}

sound_handler::~sound_handler()
{

}


bool sound_handler::setSoundPath(const QString &soundPath)
{
    this->clear();
    this->append(soundPath);
    if(!this->isEmpty()) return true;
    else return false;

    return false;
}

bool sound_handler::setSoundPath(const char *soundPath)
{
    this->clear();
    this->append(soundPath);
    if(!this->isEmpty()) return true;
    else return false;

    return false;
}

void sound_handler::setTempSoundPath(const QString &tempPath)
{
    tempSoundPath.clear();
    tempSoundPath.append(tempPath);
}

void sound_handler::setTempSoundPath(const char *tempPath)
{
    tempSoundPath.clear();
    tempSoundPath.append(tempPath);
}

void sound_handler::setVolumeLevel(int volume)
{
    if(volume>100) volumeLevel = 100;
    else if(volume<0) volumeLevel = 0;
    else volumeLevel = volume;
}

