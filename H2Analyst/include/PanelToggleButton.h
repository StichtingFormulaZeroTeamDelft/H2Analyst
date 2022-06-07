#pragma once

#include <QLabel>

class PanelToggleButton : public QLabel
{
    Q_OBJECT

public:
    enum State {
        Open, Closed
    };

private:

    uint16_t m_Width;
    QPixmap m_OpenIcon;
    QPixmap m_CloseIcon;
    State m_State;


public:

    PanelToggleButton(uint16_t width = 12, QWidget* parent = nullptr);
    void setOpenIcon(QPixmap icon) { m_OpenIcon = icon; };
    void setCloseIcon(QPixmap icon) { m_CloseIcon = icon; };
    void updateIcon();
    void toggle();
    State getState() { return m_State; };

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);

signals:
    void clicked();
};
