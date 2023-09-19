//
// Created by mi on 08.09.2023.
//

#include "Cell.h"

Cell::Cell(const QString &text, QWidget *parent) : QPushButton(text, parent) {
}

void Cell::set_up(SongGen *sg, size_t track_index, size_t buff_index, ButtonEventFilter *filter) {

    m_song_gen_ptr = sg;
    m_track_index = track_index;
    m_buff_index = buff_index;


    auto cmh = new ContextMenuHandler(this);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
            this,
            &QPushButton::customContextMenuRequested,
            cmh,
            &ContextMenuHandler::showButtonContextMenu
    );

    installEventFilter(filter);
    setAcceptDrops(true);

}

SongGen *Cell::get_song_gen_ptr() {
    return m_song_gen_ptr;
}

Track *Cell::get_track_ptr() {
    return &m_song_gen_ptr->get_file()[m_track_index];
}

size_t Cell::get_buff_index() {
    return m_buff_index;
}

void Cell::set_buff_index(size_t buff_index) {
    m_buff_index = buff_index;
}
