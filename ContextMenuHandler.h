#ifndef SONGGENGUI_CONTEXTMENUHANDLER_H
#define SONGGENGUI_CONTEXTMENUHANDLER_H

#include <iostream>

#include <QObject>
#include <QMenu>
#include <QAction>
#include "Cell.h"

#include "SongGen.h"


class ContextMenuHandler : public QWidget {
Q_OBJECT

public:

    explicit ContextMenuHandler(Cell* parent);

    void showButtonContextMenu(const QPoint &pos);

public slots:

    void muteAction();

    void copyAction();

    void pasteAction();

    void regenCleanAction();

    void regenSimilarAction();

    void regenSimilarRhythmAction();

private:
    Cell* m_cell_ptr = nullptr;

    bool m_buff_muted();
};


#endif //SONGGENGUI_CONTEXTMENUHANDLER_H
