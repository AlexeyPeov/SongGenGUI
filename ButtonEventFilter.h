//
// Created by mi on 12.08.2023.
//

#ifndef SONGGENGUI_BUTTONEVENTFILTER_H
#define SONGGENGUI_BUTTONEVENTFILTER_H
#include <QApplication>
#include <QTableWidget>
#include <QPushButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QLayout>

#include <iostream>

class ButtonEventFilter : public QObject
{
public:
    explicit ButtonEventFilter(QWidget* parent_widget, QObject* parent = nullptr)
            : QObject(parent), m_parent_widget(parent_widget), m_dragStartPosition()
    {
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
                handleMouseButtonPress(watched, event);
                break;
            case QEvent::MouseMove:
                handleMouseMove(watched, event);
                break;
            case QEvent::MouseButtonRelease:
                handleMouseButtonRelease(watched, event);
                break;
            default:
                break;
        }
        return QObject::eventFilter(watched, event);
    }

    void handleMouseButtonPress(QObject* watched, QEvent* event)
    {
        auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragStartPosition = mouseEvent->pos();
            m_draggedWidget = qobject_cast<QWidget*>(watched);
            m_initialDraggedWidgetPos = m_draggedWidget->pos();
            std::cout << "dragging..\n";
        }
    }

    void handleMouseMove(QObject* watched, QEvent* event)
    {
        auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (!(mouseEvent->buttons() & Qt::LeftButton)) {
            return;
        }
        if (!m_draggedWidget) {
            return;
        }
        updateDraggedWidgetPosition(mouseEvent);
        QWidget* targetWidget = findTargetWidget();
        swapDraggedAndTargetWidgets(targetWidget);
    }

    void handleMouseButtonRelease(QObject* watched, QEvent* event)
    {
        std::cout << "released..\n";

        if(m_draggedWidget){
            m_draggedWidget->move(m_initialDraggedWidgetPos);

            m_draggedWidget = nullptr;
        }
    }

    void updateDraggedWidgetPosition(QMouseEvent* mouseEvent)
    {
        auto btn_pos = m_draggedWidget->pos();
        auto new_x = btn_pos.x() + (mouseEvent->pos().x() - m_dragStartPosition.x());

        new_x = qMax(new_x, 0);
        new_x = qMin(new_x, m_parent_widget->width() - m_draggedWidget->width());

        btn_pos.setX(new_x);
        m_draggedWidget->move(btn_pos);
    }

    QWidget* findTargetWidget()
    {
        if(!m_draggedWidget)
            return nullptr;

        QWidget* targetWidget = nullptr;

        auto layout = m_parent_widget->layout();
        auto size = layout->count();
        auto main_box = m_draggedWidget->geometry();

        for(int i = 0; i < size; i++){

            auto curr_btn = layout->itemAt(i)->widget();

            if(curr_btn == m_draggedWidget)
                continue;

            auto curr_box = curr_btn->geometry();

            bool collision = main_box.intersects(curr_box);

            if (collision) {
                targetWidget = curr_btn;
                break;
            }

        }
        return targetWidget;
    }

    void swapDraggedAndTargetWidgets(QWidget* targetWidget)
    {
        if (!targetWidget || !m_draggedWidget)
            return;


        bool shouldSwap = false;
        QPoint pos = m_draggedWidget->pos();
        if (m_initialDraggedWidgetPos.x() < pos.x()) { // moving right
            shouldSwap = pos.x() >= targetWidget->pos().x() - (targetWidget->width() * 0.3);
        } else { // moving left
            shouldSwap = pos.x() <= targetWidget->pos().x() + (targetWidget->width() * 0.3);
        }

        if(!shouldSwap)
            return;

        auto layout = qobject_cast<QBoxLayout*>(m_parent_widget->layout());

        int targetPos = layout->indexOf(m_draggedWidget);
        int sourcePos = layout->indexOf(targetWidget);

        m_initialDraggedWidgetPos = targetWidget->pos();

        layout->insertWidget(targetPos, targetWidget);
        layout->insertWidget(sourcePos, m_draggedWidget);



    }

private:
    QWidget* m_parent_widget;
    QPoint m_dragStartPosition;
    QWidget* m_draggedWidget = nullptr;
    QPoint m_initialDraggedWidgetPos;
};
#endif //SONGGENGUI_BUTTONEVENTFILTER_H
