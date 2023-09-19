#include "ContextMenuHandler.h"


ContextMenuHandler::ContextMenuHandler(Cell* parent){
    m_cell_ptr = parent;
}

void ContextMenuHandler::showButtonContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1(m_buff_muted()? "Unmute" : "Mute", this);
    connect(&action1, &QAction::triggered, this, &ContextMenuHandler::muteAction);
    contextMenu.addAction(&action1);

    QAction action2("Copy", this);
    connect(&action2, &QAction::triggered, this, &ContextMenuHandler::copyAction);
    contextMenu.addAction(&action2);

    QAction action3("Paste", this);
    connect(&action3, &QAction::triggered, this, &ContextMenuHandler::pasteAction);
    contextMenu.addAction(&action3);

    QAction action4("Regenerate - Clean", this);
    connect(&action4, &QAction::triggered, this, &ContextMenuHandler::regenCleanAction);
    contextMenu.addAction(&action4);

    QAction action5("Regen Similar", this);
    connect(&action5, &QAction::triggered, this, &ContextMenuHandler::regenSimilarAction);
    contextMenu.addAction(&action5);

    QAction action6("Regen Similar/SameRhythm", this);
    connect(&action6, &QAction::triggered, this, &ContextMenuHandler::regenSimilarRhythmAction);
    contextMenu.addAction(&action6);

    contextMenu.exec(m_cell_ptr->mapToGlobal(pos));
}

void ContextMenuHandler::muteAction()
{
    auto& buffers = m_cell_ptr->get_track_ptr()->get_buffers();
    auto& buff = buffers[m_cell_ptr->get_buff_index()];

    if(buff.m_muted)
        buff.m_muted = false;
    else
        buff.m_muted = true;

    m_cell_ptr->get_song_gen_ptr()->set_file_to_play();
}

void ContextMenuHandler::copyAction()
{
    auto sg = m_cell_ptr->get_song_gen_ptr();

    const auto track_ptr = m_cell_ptr->get_track_ptr();
    const auto& buff = track_ptr->get_buffers()[m_cell_ptr->get_buff_index()];

    sg->insert_to_copy_buffer(buff);

    std::cout << "copy action\n";
}

void ContextMenuHandler::pasteAction()
{
    auto sg = m_cell_ptr->get_song_gen_ptr();

    auto track_ptr = m_cell_ptr->get_track_ptr();
    auto& buff = track_ptr->get_buffers()[m_cell_ptr->get_buff_index()];

    sg->extract_from_copy_buffer(buff);

    sg->set_file_to_play();


    std::cout << "paste action\n";
}

void ContextMenuHandler::regenCleanAction() {

    auto sg = m_cell_ptr->get_song_gen_ptr();
    auto track = m_cell_ptr->get_track_ptr();
    auto buff_index = m_cell_ptr->get_buff_index();

    sg->create_melody_of_track(*track, buff_index);
    sg->set_file_to_play();
}

void ContextMenuHandler::regenSimilarAction() {
    auto sg = m_cell_ptr->get_song_gen_ptr();
    auto track = m_cell_ptr->get_track_ptr();
    auto buff_index = m_cell_ptr->get_buff_index();

    sg->create_sim_melody_of_track(*track, buff_index, false);
    sg->set_file_to_play();
}

void ContextMenuHandler::regenSimilarRhythmAction(){
    auto sg = m_cell_ptr->get_song_gen_ptr();
    auto track = m_cell_ptr->get_track_ptr();
    auto buff_index = m_cell_ptr->get_buff_index();

    sg->create_sim_melody_of_track(*track, buff_index, true);
    sg->set_file_to_play();
}

bool ContextMenuHandler::m_buff_muted(){

    if(!m_cell_ptr->get_track_ptr()){
        std::cout << "TRACK IS NULLPTR AT ContextMenuHandler::m_buff_muted\n";
    }

    auto track_ptr = m_cell_ptr->get_track_ptr();
    auto& buffers = track_ptr->get_buffers();

    std::cout << "track_ptr.size(): " << track_ptr->size() << '\n';

    std::cout << "BUFF SIZE: " << buffers.size() << " buff index: " << m_cell_ptr->get_buff_index() << '\n';
    auto& buff = buffers[m_cell_ptr->get_buff_index()];
    return buff.m_muted;
}