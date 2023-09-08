//
// Created by mi on 08.09.2023.
//

#include "ContextMenuHandler.h"

ContextMenuHandler::ContextMenuHandler(QPushButton* parent){
    m_button = parent;
}

void ContextMenuHandler::showButtonContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Delete", this);
    connect(&action1, &QAction::triggered, this, &ContextMenuHandler::deleteAction);
    contextMenu.addAction(&action1);

    QAction action2("Copy", this);
    connect(&action2, &QAction::triggered, this, &ContextMenuHandler::copyAction);
    contextMenu.addAction(&action2);

    QAction action3("Paste", this);
    connect(&action3, &QAction::triggered, this, &ContextMenuHandler::pasteAction);
    contextMenu.addAction(&action3);

    contextMenu.exec(m_button->mapToGlobal(pos));
}

void ContextMenuHandler::deleteAction()
{
    std::cout << "delete action\n";
}

void ContextMenuHandler::copyAction()
{
    std::cout << "copy action\n";
}

void ContextMenuHandler::pasteAction()
{
    std::cout << "paste action\n";
}