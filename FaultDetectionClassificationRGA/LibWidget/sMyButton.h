#ifndef SMYBUTTON_H
#define SMYBUTTON_H

#include <QLabel>
#include <QPainter>
#include <QPushButton>

enum _TAB_STYLE{_TAB_SHOW,_TAB_HIDE,_TAB_ENABLE};
namespace MyWidget
{
class sMyButton:public QWidget
{
    Q_OBJECT
public:
    enum _TYPE_BUTTON{_TYPE_TAB,_TYPE_MENU,_TYPE_TITLE,_TYPE_TEXTICON,_TYPE_ICON};
    enum _STYLE_BUTTON{_STYLE_PRESS,_STYLE_MOVE,_STYLE_RELEASE,_STYLE_DISABLE};
    explicit sMyButton(QWidget *parent = nullptr);
    explicit sMyButton(QPixmap pressPicture,
                       QPixmap releasePicture,
                       QWidget *parent = nullptr);
    explicit sMyButton(QPixmap pressPicture,
                       //QSize pSize,
                       QFont pFont,
                       QWidget *parent = nullptr);
    explicit sMyButton(QPixmap pressPicture,
                       QPixmap releasePicture,
                       QPixmap movePicture,
                       QWidget *parent = nullptr);
    explicit sMyButton(QPixmap pressPicture,
                       QPixmap releasePicture,
                       QPixmap movePicture,
                       QPixmap disPicture,
                       QString text,
                       QWidget *parent = nullptr);
    void setPressPicture(QPixmap pic){
        mPressPicture = pic;
    }
    void setReleasePicture(QPixmap pic){
        mReleasePicture = pic;
    }
    void setPicture(QPixmap pressPicture,
                    QPixmap releasePicture,
                    QPixmap movePicture){
        mPressPicture = pressPicture;
        mReleasePicture= releasePicture;
        mMovePicture= movePicture;
        update();
    }
    void setPicture(QPixmap pressPicture,
                    QPixmap releasePicture,
                    QPixmap movePicture,
                    QPixmap disPicture,
                    QString text=nullptr){
        mPressPicture = pressPicture;
        mReleasePicture= releasePicture;
        mMovePicture= movePicture;
        mDisPicture= disPicture;
        if(!text.isNull())
            mText=text;
        update();
    }
    void setColor(QColor &pQColor){
        mColor = pQColor;
        update();
    }
    QColor getColor(){
        return mColor;
    }
    void setFont(QFont &pFont){
        mFont = pFont;
        update();
    }
    void setFontSize(int pointSize){
        mFont.setPointSize(pointSize);
        update();
    }
    void setTextColor(QColor disableColor=QColor(195,195,195),
                      QColor pressColor=Qt::black,
                      QColor moveColor=QColor(0,200,250),
                      QColor releaseColor=Qt::black){
        mDisableColor= disableColor;
        mPressColor= pressColor;
        mMoveColor= moveColor;
        mReleaseColor= releaseColor;
        update();
    }
    void setTextSize(int width,
                     int height,
                     int verticalOffset=0, //
                     int horizontalOffset=0,
                     int textFlags= Qt::AlignCenter){
        mSize= mPressPicture.size();
        if(width> mSize.width())
            mSize.setWidth(width);
        if(height> mSize.height())
            mSize.setHeight(height);
        mVerticalOffsetText= verticalOffset;
        mHorizontalOffsetText= horizontalOffset;
        mTextFlags= textFlags;
        setSize(mSize);
        update();
    }
    void setText(QString text){
        mText= text;
        update();
    }
    QString getText(){
        return mText;
    }
    void setSize(QSize iconSize){
        mSize=iconSize;
        //setMaximumSize(iconSize);
        setFixedSize(iconSize);
        update();
    }
    void setStyle(_TAB_STYLE TAB_STYLE1){
        m_TAB_STYLE = TAB_STYLE1;
        update();
    }
    _TAB_STYLE getStyle(){
        return m_TAB_STYLE;
    }
    void setTypeButton(_TYPE_BUTTON TYPE_BUTTON){
        m_TYPE_BUTTON = TYPE_BUTTON;
        update();
    }
    _TYPE_BUTTON getTypeButton(){
        return m_TYPE_BUTTON;
    }
    void setEnabled(bool enable){
        if(enable){
            mLastStyle= m_STYLE_BUTTON= _STYLE_RELEASE;
        }else{
            mLastStyle= m_STYLE_BUTTON= _STYLE_DISABLE;
        }
        update();
        //return QWidget::setEnabled(isEnabled);
    }
    bool isEnabled(){
        if(m_STYLE_BUTTON== _STYLE_DISABLE)
            return false;
        else
            return true;
    }



    _STYLE_BUTTON mLastStyle;
    void mousePressEvent (QMouseEvent *event){
        Q_UNUSED(event)
        if(m_STYLE_BUTTON == _STYLE_DISABLE)
            return;
        mLastStyle=m_STYLE_BUTTON=_STYLE_PRESS;
        update();
        emit ButtonClicked();//(void*)this
    }
    //    void mouseMoveEvent(QMouseEvent *event){
    //        Q_UNUSED(event)
    //        m_STYLE_BUTTON=_STYLE_MOVE;
    //        update();
    //    }
    void mouseReleaseEvent (QMouseEvent *event){
        Q_UNUSED(event)
        if(m_STYLE_BUTTON == _STYLE_DISABLE)
            return;
        mLastStyle=m_STYLE_BUTTON=_STYLE_RELEASE;
        update();
    }
    void enterEvent(QEvent *event){
        Q_UNUSED(event)
        if(m_STYLE_BUTTON == _STYLE_DISABLE)
            return;
        mLastStyle=m_STYLE_BUTTON;
        m_STYLE_BUTTON=_STYLE_MOVE;
        update();
    }
    void leaveEvent(QEvent *event){
        Q_UNUSED(event)
        if(m_STYLE_BUTTON == _STYLE_DISABLE)
            return;
        m_STYLE_BUTTON=mLastStyle;
        update();
    }

signals:
    void ButtonClicked();

private:
    //QPainter* pPaintBK;
    _TYPE_BUTTON m_TYPE_BUTTON = _TYPE_TAB;
    QFont mFont;
    QColor mFontColor = Qt::black;
    QColor mColor = Qt::red;
    QString mText;
    QPixmap mPressPicture;
    QPixmap mReleasePicture;
    QPixmap mMovePicture;
    QPixmap mDisPicture;
    QPixmap mStylePic;
    QSize mSize;
    int mVerticalOffsetText= 0; //
    int mHorizontalOffsetText= 0;
    _TAB_STYLE m_TAB_STYLE = _TAB_SHOW;
    _STYLE_BUTTON m_STYLE_BUTTON=_STYLE_RELEASE;
    //QLabel mQLabel;
    QColor mDisableColor=QColor(195,195,195),
    mPressColor=Qt::black,
    mMoveColor=QColor(0,200,250),
    mReleaseColor=Qt::black;
    int mTextFlags= Qt::AlignCenter;
    void paintEvent(QPaintEvent *);
};
}


#endif // SMYBUTTON_H
