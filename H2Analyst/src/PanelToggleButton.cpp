#include "PanelToggleButton.h"


PanelToggleButton::PanelToggleButton(uint16_t width, QWidget* parent) : QLabel(parent),
m_State(PanelToggleButton::Open),
m_Width((uint16_t)width / 2)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setMaximumWidth(width);
}

void PanelToggleButton::toggle()
{
    switch (m_State)
    {
    case Open:
        m_State = Closed;
        break;
    case Closed:
        m_State = Open;
        break;
    }
    updateIcon();
}

void PanelToggleButton::updateIcon()
{
    switch (m_State)
    {
    case Open:
        this->setPixmap(m_CloseIcon.scaledToWidth(m_Width, Qt::SmoothTransformation));
        break;
    case Closed:
        this->setPixmap(m_OpenIcon.scaledToWidth(m_Width, Qt::SmoothTransformation));
        break;
    }
}

void PanelToggleButton::enterEvent(QEvent* event)
{
    setCursor(Qt::PointingHandCursor);
}

void PanelToggleButton::leaveEvent(QEvent* event)
{
    setCursor(Qt::ArrowCursor);
}


void PanelToggleButton::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}
