//
// Created by mi on 08.09.2023.
//

#ifndef SONGGENGUI_CONTEXTMENUHANDLER_H
#define SONGGENGUI_CONTEXTMENUHANDLER_H

#include <iostream>

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QPushButton>

class ContextMenuHandler : public QWidget {
Q_OBJECT

public:

    explicit ContextMenuHandler(QPushButton* parent);

    void showButtonContextMenu(const QPoint &pos);

public slots:

    void deleteAction();

    void copyAction();

    void pasteAction();

private:
    QPushButton* m_button = nullptr;
};


#endif //SONGGENGUI_CONTEXTMENUHANDLER_H
