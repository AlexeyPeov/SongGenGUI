#ifndef SONGGENGUI_CELL_H
#define SONGGENGUI_CELL_H

#include <QPushButton>

#include "ButtonEventFilter.h"
#include "ContextMenuHandler.h"

class SongGen;
class Track;
class ContextMenuHandler;

class Cell : public QPushButton {
Q_OBJECT

public:
    explicit Cell(const QString &text, QWidget *parent = nullptr);

    void set_up(SongGen* sg, size_t track_index, size_t buff_index, ButtonEventFilter* filter);

    SongGen* get_song_gen_ptr();

    Track* get_track_ptr();

    size_t get_buff_index();

    void set_buff_index(size_t buff_index);

private:
    SongGen* m_song_gen_ptr = nullptr;
    size_t m_track_index = 0;
    size_t m_buff_index = 0;
};


#endif //SONGGENGUI_CELL_H
