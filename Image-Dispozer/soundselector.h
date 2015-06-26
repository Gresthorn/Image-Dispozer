#ifndef SOUNDSELECTOR_H
#define SOUNDSELECTOR_H

#include <QDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QUrl>


#include "rolestring.h"
#include "sound_handler.h"

namespace Ui {
class SoundSelector;
}

class SoundSelector : public QDialog
{
    Q_OBJECT

public:
    explicit SoundSelector(QVector<roleString * > * roles_list, QList<sound_handler * > * sound_items,
                           QStringList *imported_sounds, bool * ok, QWidget * parent = 0);
    ~SoundSelector();

private:
    Ui::SoundSelector *ui;
    QVector<roleString * > * rolesList;
    QList<sound_handler * > * soundItems;
    QStringList * importedSounds;

    sound_handler * preview;
    sound_handler * original_handler;

    bool *ok_s;

    qint64 default_volume_level;

    QMediaPlayer * mediaPlayer;

    void switchToAnotherParent(QTreeWidgetItem * item, int index);
    void setNewMedia(const QString & path_to_media);

protected:
    void keyPressEvent(QKeyEvent * e);

public slots:
    void linkSelectedItem(void);
    void unlinkSelectedItems(void);
    void loadSelectedSounds(void);
    void loadSoundsFromDir(void);
    void deleteSelectedSounds(void);
    void soundListWasClicked(void);
    void soundRoleWasClicked(void);

    void cancelButtonClicked(void);
    void acceptButtonClicked(void);

    void volumeChangedSlot(int new_pos);
    void progressChangedSlot(qint64 new_pos);
    void progressUserChangedSlot(int new_pos);
    void durationChangedSlot(qint64 new_maximum);
    void playButtonPressedSlot(void);
    void stopButtonPressedSlot(void);

    void disablePlayer(void);
    void enablePlayer(QString filename);
};

#endif // SOUNDSELECTOR_H
