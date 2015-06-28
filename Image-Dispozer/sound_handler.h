#ifndef SOUND_HANDLER_H
#define SOUND_HANDLER_H

#include <QString>


class sound_handler : public QString
{

private:
    QString tempSoundPath;
    int indexPosition;
    int soundRole;
    int volumeLevel;
    bool enabled;

public:
    sound_handler(const char *ch, int roleCode = 0, int index = 0, int volume = 5);
    sound_handler(const QString &s,  int roleCode = 0, int index = 0, int volume = 5);
    ~sound_handler();

    bool setSoundPath(const QString & soundPath);
    bool setSoundPath(const char * soundPath);
    QString getSoundPath(void) { return static_cast<QString>(*this); }
    void setTempSoundPath(const QString & tempPath);
    void setTempSoundPath(const char * tempPath);
    QString getTempSoundPath(void) { return tempSoundPath; }

    int getSoundRole(void) { return soundRole; }
    int getIndex(void) { return indexPosition; }

    void clearTempSoundPath(void) { tempSoundPath.clear(); }
    void discardSound(void) { this->clear(); }

    void setVolumeLevel(int volume);
    int getVolumeLevel(void) { return volumeLevel; }

    bool isEnabled(void) { return enabled; }
    void setEnabled(bool enable) { enabled = enable; }
};

#endif // SOUND_HANDLER_H
