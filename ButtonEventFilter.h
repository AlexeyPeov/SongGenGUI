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

#include "SongGen.h"

class Cell;

class ButtonEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit ButtonEventFilter(QWidget* parent_widget, QObject* parent = nullptr, SongGen* sg = nullptr);


protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

    void handleMouseButtonPress(QObject* watched, QEvent* event);

    void handleMouseMove(QObject* watched, QEvent* event);

    void handleMouseButtonRelease(QObject* watched, QEvent* event);

    void updateDraggedWidgetPosition(QMouseEvent* mouseEvent);

    Cell* findTargetWidget();

    void swapDraggedAndTargetWidgets(Cell* targetWidget);

private:
    QWidget* m_parent_widget;
    QPoint m_dragStartPosition;
    Cell* m_draggedWidget = nullptr;
    QPoint m_initialDraggedWidgetPos;

    SongGen* m_song_gen_ptr = nullptr;
};
#endif //SONGGENGUI_BUTTONEVENTFILTER_H
