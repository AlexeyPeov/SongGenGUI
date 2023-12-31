//
// Created by mi on 05.08.2023.
//
#ifndef SONGGENGUI_APP_H
#define SONGGENGUI_APP_H

#include <memory>
#include <thread>

#include <QDialog>
#include <QLineEdit>
#include <QRegularExpression>
#include <QTimer>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>


#include "ContextMenuHandler.h"
#include "ButtonEventFilter.h"
#include "CustomSlider.h"
#include "SliderHeader.h"
#include "Cell.h"

#include "../SongGen/SongGenEnumsToString.h"
#include "ContextMenuHandler.h"
#include "SongGen.h"

class File;
class SongGen;

QT_BEGIN_NAMESPACE
namespace Ui { class App; }
QT_END_NAMESPACE

class App : public QDialog {
Q_OBJECT

public:
    explicit App(QWidget *parent = nullptr);

    ~App() override;

private slots:

    // top widgets
    void on_styleComboBox_currentIndexChanged(int index);

    void on_chordProgressionComboBox_currentIndexChanged(int index);

    void on_moodComboBox_currentIndexChanged(int index);

    void on_keyComboBox_currentIndexChanged(int index);

    void on_tempoSliderValueChanged(int value);

    // tab 1 bottom buttons
    void on_addRowButton_clicked();

    void on_removeRowButton_clicked();

    void on_createButton_clicked();


    // tab 2 buttons
    void on_playAllButton_clicked();

    void on_playAllButton_pressed();

    void on_stopAllButton_clicked();

    void on_exportButton_clicked();

    void on_addChunkButton_clicked();

    void on_removeChunkButton_clicked();

    // tab 2 table resize

    void resizeEvent(QResizeEvent *event);


private:
    Ui::App *ui;
    std::unique_ptr<SongGen> m_song_gen_ptr;
    //Player* player = nullptr;
    std::unique_ptr<File> m_file_to_play_ptr;
    std::vector<File> m_created_files;
    std::thread m_play_thread;


    void m_add_chunk();


    void on_sliderPressed();

    void on_sliderReleased();

    void on_sliderValueChanged(int value);


    void
    setup_row(int start_oct, int range, ProgressionStyle prog_st,
              NoteLenBias nlb, uint32_t custom_note_len, int instr_index,
              int default_table_row_index);

    void m_add_track_to_timeline_table(size_t track_index);

    void m_add_track_chunk(size_t track_index, QWidget *widget);

    void m_pop_track_chunk(Track &track, QWidget *widget);

    void m_pop_chunk();
};

#endif //SONGGENGUI_APP_H
