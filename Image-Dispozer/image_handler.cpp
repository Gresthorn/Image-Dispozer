#include "image_handler.h"

image_handler::image_handler(QString path, int roleCode, int index, image_groups group)
    : QPixmap(), PI(3.141592653589793238462643383279502884197169399375105820974944592307816406286)
{
    indexPosition = index;

    group_id =group;

    tempImagePath.clear();
    imagePath.clear();
    imagePath.append(path);

    imageRole = roleCode;

    if(!path.isEmpty())
        fileOK = loadImage(imagePath);
    else fileOK = false;

    currentlyDisplayed = false;

    docked = true;

    // only implicit values
    xyPosition.setX(0.0); xyPosition.setY(0.0);
    lbCorner.setX(-100.0); lbCorner.setY(-100.0);
    itemSize.setWidth(100.0); itemSize.setHeight(100.0);
    itemRotation = 0.0;
}

image_handler::image_handler(QPixmap img, QString path, int roleCode, int index, bool file_correct, image_groups group)
    : QPixmap(img), PI(3.141592653589793238462643383279502884197169399375105820974944592307816406286)
{
    // We suppose, that 'img' is pixmap already loaded in memory, so we just use this pixmap
    // without checking if file exists in the path given.

    indexPosition = index;

    group_id = group;

    tempImagePath.clear();
    imagePath.clear();
    imagePath.append(path);

    imageRole = roleCode;

    currentlyDisplayed = false;

    fileOK = file_correct;

    docked = true;

    // only implicit values
    xyPosition.setX(0.0); xyPosition.setY(0.0);
    lbCorner.setX(-100.0); lbCorner.setY(-100.0);
    itemSize.setWidth(100.0); itemSize.setHeight(100.0);
    itemRotation = 0.0;
}

image_handler &image_handler::operator=(image_handler &image)
{
    QPixmap temp = image.copy();
    image_handler * new_handler = new image_handler(temp, image.getImagePath(), image.getImageRole(),
                                                    image.getIndex(), image.isFileCorrect(), image.getImageGroup());

    // update position and size of newly created item
    new_handler->setItemSize(this->getItemSize());
    new_handler->setPosition(this->getPosition());
    new_handler->setLBCorner(this->getLBCorner());
    new_handler->setItemRotation(this->getItemRotation());

    return *new_handler;
}

void image_handler::setItemRotation(qreal angle)
{
    QRectF wrapper = calculateWrapperCorners();
    qreal wrapperWidth = wrapper.width()>0.0 ? wrapper.width() : -wrapper.width();
    qreal wrapperHeight = wrapper.height()>0.0 ? wrapper.height() : -wrapper.height();
    this->setLBCorner(QPointF(xyPosition.x()-wrapperWidth/2.0, xyPosition.y()-wrapperHeight/2.0));
    itemRotation = angle;
}

QRectF image_handler::calculateWrapperCorners(qreal angle)
{
    // this setting allows to call this function with desired angle and calculate
    // resulting wrapper without needing to modify itemRotation of handler (usefull for predictions)
    qreal rot_angle = (angle>=0.0) ? angle : itemRotation;

    qreal height = itemSize.height();
    qreal width = itemSize.width();

    // calculate diagonal line length between [0, 0] and corner
    qreal diag_length = sqrt(pow(width/2.0, 2.0)+pow(height/2.0, 2.0));
    qreal offset_angle_right = asin((height/2.0)/diag_length);
    qreal offset_angle_left = PI-offset_angle_right;

    qreal r_angle_radians = (rot_angle/180.0)*PI;

    // change angles in 2nd and 4th quadrant
    if((r_angle_radians>=PI/2.0 && r_angle_radians<PI) ||
            ((r_angle_radians>=((PI/2.0)*3.0)) && r_angle_radians<(2.0*PI))) r_angle_radians = (PI/2.0-r_angle_radians)+PI/2.0;

    qreal calc_angle_right = offset_angle_right+r_angle_radians;
    qreal calc_angle_left = offset_angle_left+r_angle_radians;

    //qreal top_right_x = cos(calc_angle_right)*diag_length;
    //qreal top_left_y = sin(calc_angle_left)*diag_length

    qreal top_right_y = sin(calc_angle_right)*diag_length;
    qreal top_left_x = cos(calc_angle_left)*diag_length;

    // make correction for angles greater or equal to 180.0 otherwise rect will be mirrored and flipped
    if(itemRotation>=180.0)
    {
        top_right_y = -top_right_y;
        top_left_x = -top_left_x;
    }

    QPointF topLeftW, topRightW, bottomLeftW, bottomRightW;

    // calculate reference corners
    topLeftW.setX(top_left_x);
    topLeftW.setY(top_right_y);

    bottomRightW.setX(-top_left_x);
    bottomRightW.setY(-top_right_y);

    // calculate other corners
    /*topRightW.setX(-top_left_x);
    topRightW.setY(top_right_y);

    bottomLeftW.setX(top_left_x);
    bottomLeftW.setY(-top_right_y);*/

    QRectF wrapperRect(topLeftW, bottomRightW);

    return wrapperRect;
}

int image_handler::setImage(QString path)
{
    if(loadImage(path))
    {
        // if load of image was successful, new image can be set
        imagePath.clear();
        imagePath.append(path);
        fileOK = true;

        return 1;
    }
    else
    {
        // New image could not be loaded, but we lost the old one, so we
        // need to reload an old image. If old image could not be loaded,
        // path is set back to empty string.
        fileOK = loadImage(imagePath);
        if(!fileOK)
        {
            imagePath.clear();

            return -1;
        }

        return 0;
    }
}

void image_handler::discardImage()
{
    // set default values
    fileOK = false;
    imagePath.clear();
}

bool image_handler::loadImage(QString imgPath)
{
    QFileInfo file(imgPath);
    if(file.exists() && file.isFile())
    {
        // check for images in format jpg, bmp or png
        QString suffix = file.completeSuffix();
        if(suffix=="bmp" || suffix=="jpg" || suffix=="png")
        {
            QImage tempImage;
            if(tempImage.load(imgPath))
            {
                *(static_cast<QPixmap * >(this)) = fromImage(tempImage);
                this->setItemSize(QSizeF(tempImage.width(), tempImage.height()));
                return true;
            }
            else
            {
                // If image is in bmp format, try to load with 16bit bmp (5-6-5) function.
                // This special format, if is generated in e.g. Photoshop, the produced
                // header is not supported by Qt yet. This method will try to load image
                // by reading each byte one by one and converting to QPixmap manually.
                if(suffix=="bmp")
                {
                    QImage * tempImage = load16bitBmp565(imgPath, true);
                    if(tempImage!=NULL) // the load was successful
                    {
                        *(static_cast<QPixmap * >(this)) = fromImage(*tempImage);
                        this->setItemSize(QSizeF(tempImage->width(), tempImage->height()));
                        delete tempImage;

                        return true;
                    }
                    else return false; // function was not able to load bmp image
                }
            }
        }
        else return false;
    }
    else return false;

    return false;
}

QImage * image_handler::load16bitBmp565(QString file, bool bigEndian)
{
    const int FILEHEADERSIZE = 14;

    QFile imgFile(file); // open image file (in bmp format)
    if(!imgFile.open(QFile::ReadOnly)) return NULL;

    if(!imgFile.seek(14)) return NULL; // skip FILEHEADER

    char * tempData = new char[4]; // temporary array for reading 4 bytes from header

    // extract INFOHEADER size
    imgFile.read(tempData,4); // get header size from file (bytes 14-17 in bmp standard header)
    int info_header_size = char2int(tempData, bigEndian); // 56 is value for INFOV3HEADER (unsupported by Qt so far)

    imgFile.read(tempData, 4); // extract image width (bytes 18-21)
    int width = char2int(tempData, bigEndian);
    imgFile.read(tempData, 4); // extract image height (bytes 22-25)
    int height = char2int(tempData, bigEndian);

    // skip FILEHEADER and INFOHEADER to read just pixel array data
    if(!imgFile.seek(FILEHEADERSIZE+info_header_size)) return NULL;

    // pointer is set to first pixel, so we can simply read the rest (16 bit values - two bytes per pixel)
    QByteArray imgArray = imgFile.read(height*width*2);
    const uchar * imgPixelData = (uchar *)imgArray.data();

    // create image object to extract pixel data array
    QImage * img = new QImage(imgPixelData, width, height, QImage::Format_RGB16);
    if(img->isNull()) return NULL;

    // create QPixmap, so image can be used later in graphics scene

    if(bigEndian)
    {
        // if big endian approach is used, also top left ixel is read as last
        // simple vertical flip can be used to make correctly displayed image
        *img = img->mirrored(false, true); // transform image (horizontal flip)
    }

    delete [] tempData;
    //delete [] imgPixelData;

    imgFile.close();

    return img;
}

int image_handler::char2int(char *array, bool bigEndian)
{
    int result = 0;
    uchar t; // for correct, unsigned interpretation of 1 byte
    if(bigEndian) // big endian approach
        for(int i=4; i>0; i--)
        {
            t = array[i-1];
            result = result << 8;
            result = result | t;
        }
    else // little endian approach
        for(int i=0; i<4; i++)
        {
            t = array[i];
            result = result << 8;
            result = result | t;
        }

    return result;
}

