#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <QPixmap>
#include <QImage>
#include <QDebug>
#include <QString>
#include <QFileInfo>

class image_handler : public QPixmap
{

private:
    QString imagePath;
    int imageRole;
    int indexPosition;

    bool fileOK;
    bool changed;

    bool loadImage(QString imgPath);
    QImage * load16bitBmp565(QString file, bool bigEndian);
    int char2int(char * array, bool bigEndian);

public:
    image_handler(QString path = QString(""), int roleCode = 0, int index = 0);
    image_handler(QPixmap img, QString path, int roleCode, int index, bool file_correct = false);

    image_handler &operator=(image_handler &image);

    QString getImagePath(void) { return imagePath; }
    int getImageRole(void) { return imageRole; }
    int getIndex(void) { return indexPosition; }
    bool isFileCorrect(void) { return fileOK; }

    void setImage(QString path);
    void discardImage(void);
    bool isChanged(void);
};

#endif // IMAGE_HANDLER_H