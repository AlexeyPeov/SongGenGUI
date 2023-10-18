//
// Created by mi on 05.08.2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_App.h" resolved


#include "App.h"
#include "ui_App.h"


App::App(QWidget *parent) :
        QDialog(parent), ui(new Ui::App) {
    ui->setupUi(this);

//    QMessageBox msgBox(this);
//    msgBox.setText("Setting everything up..");
//    msgBox.exec();

    m_song_gen_ptr = std::make_unique<SongGen>();
    m_song_gen_ptr->set_delta_time(converters::microseconds_to_delta_time(500000,500000,500));

    m_song_gen_ptr->pause_playing();

    m_play_thread = std::thread([this] { m_song_gen_ptr->play_next_event(); });

    QHeaderView *header1 = ui->createTableWidget->horizontalHeader();
    header1->setSectionResizeMode(QHeaderView::Stretch);



    //ui->playSlider->setStyleSheet(style);

    //auto *customSlider = new CustomSlider(ui->timelineTableWidget);
    //customSlider->setGeometry(ui->playSlider->geometry());
    //customSlider->setOrientation(ui->playSlider->orientation());
    //customSlider->setStyleSheet(ui->playSlider->styleSheet());
    //customSlider->setOrientation(Qt::Horizontal);
    //customSlider->setRange(0, 999);

    //QLayout *layout = ui->playSlider->parentWidget()->layout();
    //layout->replaceWidget(ui->playSlider, customSlider);

    //delete ui->playSlider;
    //ui->playSlider = customSlider;

    int columnWidth = ui->timelineTableWidget->width() / 10;
    ui->timelineTableWidget->setColumnWidth(0, columnWidth);

    auto slider_header = new SliderHeader(Qt::Horizontal, ui->timelineTableWidget);
    auto slider = slider_header->getSlider();

    ui->timelineTableWidget->setHorizontalHeader(slider_header);

    slider_header->setSectionResizeMode(0, QHeaderView::Fixed);
    slider_header->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(slider, &CustomSlider::valueChanged, this, &App::on_sliderValueChanged);
    //connect(slider, &CustomSlider::sliderPressed, this, &App::on_sliderPressed);
    /*QPainter painter(this);
        style()->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);

        QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        QPoint handleCenter = handleRect.center();
        QPoint bottomCenter(opt.rect.center().x(), opt.rect.bottom());
        painter.drawLine(handleCenter, bottomCenter);*/

    //connect(ui->playSlider, &QSlider::paintEvent, )

//    connect(ui->timelineTableWidget->horizontalHeader(), &QHeaderView::sectionResized, this, [&](int logicalIndex, int oldSize, int newSize){
//            if (logicalIndex == 0) {
//                ui->playSlider->setFixedWidth(newSize);
//                int columnX = ui->timelineTableWidget->columnViewportPosition(1);
//                QPoint columnPos(columnX, 0);
//                QPoint globalPos = ui->timelineTableWidget->mapToGlobal(columnPos);
//                int absoluteX = globalPos.x();
//
//                ui->playSlider->move(absoluteX, ui->playSlider->pos().y());
//            }
//    });


    //player = new Player(&output);


    auto timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, [&] {
        auto mult = m_song_gen_ptr->get_current_playback_position();

        auto header = (SliderHeader*)ui->timelineTableWidget->horizontalHeader();
        auto slider = header->getSlider();


        double max_pos = slider->maximum();
        slider->blockSignals(true);
        int pos = max_pos * mult;

        if(pos >= slider->maximum()){
            slider->setSliderPosition(0);
            m_song_gen_ptr->pause_playing();

            m_song_gen_ptr->set_play_position(
                    slider->minimum(),
                    slider->maximum(),
                    0);
        }
        else {
            slider->setSliderPosition(pos);
        }

        slider->blockSignals(false);
    });
    timer->start(5);


    for (auto &[str, en]: style_vec)
        ui->styleComboBox->addItem(QString::fromStdString(str));

    for (auto &[str, en]: keys_vec)
        ui->keyComboBox->addItem(QString::fromStdString(str));

    for (auto &[str, en]: mood_vec)
        ui->moodComboBox->addItem(QString::fromStdString(str));

    bool is_major = ui->moodComboBox->currentText() == "Major";
    if(is_major){
        ui->chordProgressionComboBox->clear();
        for(auto& str : chords_maj_vec){
            ui->chordProgressionComboBox->addItem(QString::fromStdString(str));
        }
    }
    else{
        ui->chordProgressionComboBox->clear();
        for(auto& str : chords_min_vec){
            ui->chordProgressionComboBox->addItem(QString::fromStdString(str));
        }
    }

    auto dt = m_song_gen_ptr->dt();

    const size_t grand_piano = 15;

    on_addRowButton_clicked();
    setup_row(3, 0, ProgressionStyle::CHORD_PLAIN, NoteLenBias::NO, 0, grand_piano, 0);

    // was - 19 41 53 95

    on_addRowButton_clicked();
    setup_row(3, 0, ProgressionStyle::ROOTS, NoteLenBias::SLOW, dt * 4, grand_piano, 1);

    on_addRowButton_clicked();
    setup_row(3, 0, ProgressionStyle::NOTES_OF_CHORD, NoteLenBias::SLOW, 0, grand_piano, 2);

    on_addRowButton_clicked();
    setup_row(4, 0, ProgressionStyle::NOTES, NoteLenBias::FAST, 0, grand_piano, 3);


    auto tempoSlider = ui->tempoSlider;
    tempoSlider->setRange(60,200);
    tempoSlider->setValue(120);

    connect(tempoSlider, &QSlider::valueChanged, this, &App::on_tempoSliderValueChanged);

}

App::~App() {
    m_song_gen_ptr->pause_playing();
    m_song_gen_ptr->terminate_playback();
    m_play_thread.join();
    delete ui;
}

void App::on_tempoSliderValueChanged(int value){
    auto bpmLabel = ui->bpmLabel;
    auto str = "BPM: " + std::to_string(value);
    bpmLabel->setText(str.c_str());
    m_song_gen_ptr->set_bpm(value);
}

void App::on_styleComboBox_currentIndexChanged(int index) {
    auto str = ui->styleComboBox->currentText().toStdString();
    auto it = std::find_if(style_vec.begin(), style_vec.end(), [&](auto& pair){ return pair.first == str; });
    auto&[k,style] = *it;
    m_song_gen_ptr->set_style(style);
}

void App::on_chordProgressionComboBox_currentIndexChanged(int index){
    Q_UNUSED(index)

    auto str = ui->chordProgressionComboBox->currentText().toStdString();

    const auto& vec = m_song_gen_ptr->mood_is_major() ? chords_maj_vec : chords_min_vec;


    auto it = std::find_if(
            vec.begin(),
            vec.end(),
            [&](auto& pr){ return pr == str; }
    );
    if(it != vec.end()){
        auto progression = *it;
        if(progression == "Random")
            progression = vec[m_song_gen_ptr->m_rand<size_t>(1,vec.size()-1)];

        m_song_gen_ptr->set_chord_progression(progression);

    }
}

void App::on_moodComboBox_currentIndexChanged(int index){
    Q_UNUSED(index)
    auto str = ui->moodComboBox->currentText().toStdString();
    auto it = std::find_if(
            mood_vec.begin(),
            mood_vec.end(),
            [&](auto& pair){ return pair.first == str; }
    );

    auto&[k,mood] = *it;
    m_song_gen_ptr->set_mood(mood);

    bool is_major = ui->moodComboBox->currentText() == "Major";
    if(is_major){
        ui->chordProgressionComboBox->clear();
        for(auto& pr : chords_maj_vec){
            ui->chordProgressionComboBox->addItem(QString::fromStdString(pr));
        }
    }
    else{
        ui->chordProgressionComboBox->clear();
        for(auto& pr : chords_min_vec){
            ui->chordProgressionComboBox->addItem(QString::fromStdString(pr));
        }
    }

}

void App::setup_row(int start_oct, int range, ProgressionStyle prog_st,
                    NoteLenBias nlb, uint32_t custom_note_len, int instr_index,
                    int default_table_row_index) {
    auto table = ui->createTableWidget;

    auto st_oct_wid = (QComboBox*)table->cellWidget(default_table_row_index,1);
    auto range_wid = (QComboBox*)table->cellWidget(default_table_row_index, 2);
    auto prog_st_wid = (QComboBox*)table->cellWidget(default_table_row_index,3);
    auto nlb_wid = (QComboBox*)table->cellWidget(default_table_row_index,4);
    auto cust_note_len_wid = (QComboBox*)table->cellWidget(default_table_row_index,5);
    auto instr_wid = (QComboBox*)table->cellWidget(default_table_row_index,6);

    st_oct_wid->setCurrentIndex(start_oct);
    range_wid->setCurrentIndex(range);
    prog_st_wid->setCurrentIndex((int)progression_style_vec[(int)prog_st].second);
    nlb_wid->setCurrentIndex((int)note_len_bias_vec[(int)nlb].second);


    auto dt = m_song_gen_ptr->dt();

    if(custom_note_len == 0){
        cust_note_len_wid->setCurrentIndex(0);
    }
    else if(custom_note_len == dt / 4){
        cust_note_len_wid->setCurrentIndex(1);
    }
    else if(custom_note_len == dt / 2){
        cust_note_len_wid->setCurrentIndex(2);
    }
    else if(custom_note_len == dt){
        cust_note_len_wid->setCurrentIndex(3);
    }
    else if(custom_note_len == dt * 2){
        cust_note_len_wid->setCurrentIndex(4);
    }
    else if(custom_note_len == dt * 4){
        cust_note_len_wid->setCurrentIndex(5);
    }

    instr_wid->setCurrentIndex(instr_index);
}

void App::on_keyComboBox_currentIndexChanged(int index){
    Q_UNUSED(index)

    auto str = ui->keyComboBox->currentText().toStdString();
    auto it = std::find_if(
            keys_vec.begin(),
            keys_vec.end(),
            [&](auto& pair){ return pair.first == str; }
    );

    auto&[k,key] = *it;
    m_song_gen_ptr->set_key(key);
}

void App::on_addRowButton_clicked() {

    m_song_gen_ptr->add_track();
    const int row = ui->createTableWidget->rowCount();
    ui->createTableWidget->insertRow(row);
    auto& track = m_song_gen_ptr->get_track((size_t)row);


    auto nameLineEdit = new QLineEdit;
    QRegularExpression rx("[\\x00-\\x7F]*");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    nameLineEdit->setValidator(validator);
    QString track_name = "track_" + QString::fromStdString(std::to_string(row+1));
    nameLineEdit->setText(track_name);
    track.set_name(track_name.toStdString());
    ui->createTableWidget->setCellWidget(row, 0, nameLineEdit);

    connect(nameLineEdit, &QLineEdit::textEdited, this, [this, nameLineEdit, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = nameLineEdit->text().toStdString();
        track.set_name(str);
    });


    auto startOctaveComboBox = new QComboBox;
    for(int i = 1; i < 9; i++)
        startOctaveComboBox->addItem(QString(std::to_string(i).c_str()));

    track.set_start_octave(4);
    startOctaveComboBox->setCurrentText("4");
    ui->createTableWidget->setCellWidget(row, 1, startOctaveComboBox);

    connect(startOctaveComboBox, &QComboBox::currentIndexChanged, this, [this, startOctaveComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto num = startOctaveComboBox->currentText().toUInt();
        track.set_start_octave(num);
    });


    auto rangeComboBox = new QComboBox;
    for(int i = 1; i < 6; i++)
        rangeComboBox->addItem(QString(std::to_string(i).c_str()));
    ui->createTableWidget->setCellWidget(row, 2, rangeComboBox);

    track.set_range_in_octaves(0);

    connect(rangeComboBox, &QComboBox::currentIndexChanged, this, [this, rangeComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto num = rangeComboBox->currentText().toUInt();
        track.set_range_in_octaves(num - 1);
    });


    auto progrStyleComboBox = new QComboBox;

    for(auto&[str, en] : progression_style_vec)
        progrStyleComboBox->addItem(QString::fromStdString(str));

    ui->createTableWidget->setCellWidget(row, 3, progrStyleComboBox);

    connect(progrStyleComboBox, &QComboBox::currentIndexChanged, this, [this, progrStyleComboBox, row](int index){
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = progrStyleComboBox->currentText().toStdString();
        auto&[k,progrStyle] = progression_style_vec[index];
        track.set_progression_style(progrStyle);
    });


    auto noteLenBiasComboBox = new QComboBox;
    for(auto&[str, en] : note_len_bias_vec)
        noteLenBiasComboBox->addItem(QString::fromStdString(str));
    ui->createTableWidget->setCellWidget(row, 4, noteLenBiasComboBox);

    connect(noteLenBiasComboBox, &QComboBox::currentIndexChanged, this, [this, noteLenBiasComboBox, row](int index){
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = noteLenBiasComboBox->currentText().toStdString();
        auto&[k,noteLenBias] = note_len_bias_vec[index];
        track.set_note_len_bias(noteLenBias);
    });

    auto customNoteLenComboBox = new QComboBox;
    customNoteLenComboBox->addItem("No");
    customNoteLenComboBox->addItem("Sixteenth");
    customNoteLenComboBox->addItem("Eigth");
    customNoteLenComboBox->addItem("Quarter");
    customNoteLenComboBox->addItem("Half");
    customNoteLenComboBox->addItem("Whole");
    ui->createTableWidget->setCellWidget(row, 5, customNoteLenComboBox);

    connect(customNoteLenComboBox, &QComboBox::currentIndexChanged, this, [this, customNoteLenComboBox, row](int index){
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = customNoteLenComboBox->currentText().toStdString();
        auto dt = m_song_gen_ptr->dt();

        uint32_t note_len = 0;

        if(str == "Sixteenth")
            note_len = dt / 4;
        else if(str == "Eigth")
            note_len = dt / 2;
        else if(str == "Quarter")
            note_len = dt;
        else if(str == "Half")
            note_len = dt*2;
        else if(str == "Whole")
            note_len = dt*4;

        track.set_length_of_notes(note_len);
    });


    auto instrumentComboBox = new QComboBox;
    for(auto&[str, en] : instruments_vec)
        instrumentComboBox->addItem(QString::fromStdString(str));
    ui->createTableWidget->setCellWidget(row, 6, instrumentComboBox);

    const auto& instrument_info = instruments_vec[0];
    uint32_t size = instrument_info.second.size();
    auto rand = m_song_gen_ptr->m_rand<uint32_t>(0,size-1);
    const auto& instrument = instrument_info.second[rand];
    m_song_gen_ptr->get_track((size_t)row).set_instrument(instrument);

    connect(instrumentComboBox, &QComboBox::currentIndexChanged, this, [this, instrumentComboBox, row](int index){
        const auto& instrument_info = instruments_vec[index];
        uint32_t size = instrument_info.second.size();
        auto rand = m_song_gen_ptr->m_rand<uint32_t>(0,size-1);
        const auto& instrument = instrument_info.second[rand];
        m_song_gen_ptr->get_track((size_t)row).set_instrument(instrument);
    });

    m_add_track_to_timeline_table(row);
}

void App::on_removeRowButton_clicked() {
    int row = ui->createTableWidget->rowCount();

    bool empty = row == 0;

    if(empty)
        return;

    ui->createTableWidget->removeRow(row - 1);
    m_song_gen_ptr->pop_track();


    ui->timelineTableWidget->removeRow(row-1);
    m_song_gen_ptr->set_file_to_play();

}

void App::on_createButton_clicked() {
//    m_song_gen_ptr->print_tracks_info();
//    m_song_gen_ptr->create();
//    m_song_gen_ptr->mute_playing();
//    m_song_gen_ptr->pause_playing();
//    m_add_chunk();
//    m_song_gen_ptr->set_file_to_play();
}

void App::m_add_track_chunk(size_t track_index, QWidget* widget){

    auto& tracks = m_song_gen_ptr->get_file();
    auto& track = tracks[track_index];
    auto& buffers = track.get_buffers();

    // create row
    auto filter = new ButtonEventFilter(widget, nullptr, m_song_gen_ptr.get());
    auto layout = widget->layout();

    // add buffer to track
    buffers.emplace_back();
    auto buff_index = buffers.size() - 1;
    m_song_gen_ptr->create_melody_of_track(track, buff_index);

    auto cell = new Cell("");
    cell->set_up(m_song_gen_ptr.get(), track_index, buff_index, filter);
    layout->addWidget(cell);
}

void App::m_pop_track_chunk(Track& track, QWidget* widget){
    auto& buffers = track.get_buffers();

    bool empty = buffers.empty();

    if(empty)
        return;

    buffers.pop_back();

    int index = buffers.size();

    auto layout = widget->layout();

    QLayoutItem *item = layout->itemAt(index);
    if (item != nullptr) {
        auto w = item->widget();
        if (w != nullptr) {
            layout->removeWidget(w);
            delete w;
        }
    }

}

void App::m_add_chunk(){

    m_song_gen_ptr->pause_playing();

    auto& tracks = m_song_gen_ptr->get_file();
    const auto tracks_size = tracks.size();

    m_song_gen_ptr->set_chunk_amount(m_song_gen_ptr->get_chunk_amount() + 1);

    auto table = ui->timelineTableWidget;
    auto header = (SliderHeader*)table->horizontalHeader();
    auto slider = header->getSlider();

    auto slider_width = slider->width();

    auto column_index = 1;
    table->setColumnWidth(column_index, slider_width);

    for(size_t i = 0; i < tracks_size; i++){
        auto widget = table->cellWidget(i, column_index);
        m_add_track_chunk(i,widget);
    }

    m_song_gen_ptr->set_file_to_play();
}

void App::m_pop_chunk(){

    m_song_gen_ptr->pause_playing();

    auto chunk_am = m_song_gen_ptr->get_chunk_amount();

    bool empty = chunk_am == 0;

    if(empty)
        return;

    m_song_gen_ptr->set_chunk_amount(chunk_am - 1);


    auto& tracks = m_song_gen_ptr->get_file();
    const auto tracks_size = tracks.size();

    auto table = ui->timelineTableWidget;
    auto header = (SliderHeader*)table->horizontalHeader();
    auto slider = header->getSlider();

    auto slider_width = slider->width();

    auto column_index = 1;
    table->setColumnWidth(column_index, slider_width);

    for(auto i = 0; i < tracks_size; i++){
        auto& track = tracks[i];
        auto widget = table->cellWidget(i, column_index);
        m_pop_track_chunk(track,widget);
    }

    m_song_gen_ptr->set_file_to_play();
}

void App::m_add_track_to_timeline_table(size_t track_index) {

    auto& track = m_song_gen_ptr->get_file()[track_index];

    auto table = ui->timelineTableWidget;
    auto header = (SliderHeader*)table->horizontalHeader();
    auto slider = header->getSlider();

    auto slider_width = slider->width();

    auto column_index = 1;
    table->setColumnWidth(column_index, slider_width);


    auto& buffers = track.get_buffers();

    buffers.resize(m_song_gen_ptr->get_chunk_amount());
// todo : regenerate clean crashes - why?
    // create row
    int row = table->rowCount();
    table->insertRow(row);

    auto* widget = new QWidget;
    auto filter = new ButtonEventFilter(widget, nullptr, m_song_gen_ptr.get());

    widget->setContentsMargins(0,0,0,0);

    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);



    // add buffer to track
    for(size_t j = 0; j < buffers.size(); j++){
        m_song_gen_ptr->create_melody_of_track(track, j);
        auto cell = new Cell("");
        cell->set_up(m_song_gen_ptr.get(), track_index, j, filter);
        layout->addWidget(cell);
    }

    widget->setLayout(layout);
    table->setCellWidget(row, column_index, widget);

}

void App::on_sliderPressed(){
    std::cout << "slider pressed!\n";
//    auto header = (SliderHeader*)ui->timelineTableWidget->horizontalHeader();
//    auto slider = header->getSlider();
    //slider->setValue() // where the mouse is

}

void App::on_sliderReleased(){
    std::cout << "slider released!\n";
    //m_song_gen_ptr->resume_playing();
}

void App::on_sliderValueChanged(int value){

    if(value < 0){
        std::cout << "value less than zero at App::on_playSlider_valueChanged\n";
        return;
    }

    bool is_paused = m_song_gen_ptr->is_playback_paused();

    m_song_gen_ptr->pause_playing();

    auto header = (SliderHeader*)ui->timelineTableWidget->horizontalHeader();
    auto slider = header->getSlider();


    m_song_gen_ptr->set_play_position(
            slider->minimum(),
            slider->maximum(),
            (uint32_t)value
    );

    if(!is_paused)
        m_song_gen_ptr->resume_playing();
}

void App::on_playAllButton_clicked(){
    // this function needs to be called each update
    //m_player_ptr->play();
}

void App::on_playAllButton_pressed(){
    m_song_gen_ptr->resume_playing();
    //m_song_gen_ptr->play_moment();
}

void App::on_stopAllButton_clicked() {
    m_song_gen_ptr->pause_playing();
}

void App::on_exportButton_clicked()
{
    bool ok;
    QString dir_name = QInputDialog::getText(
            this, tr("Enter song title, a new directory will be created"
                     "\nwith all the tracks inside."),
            tr("Directory name:"),
            QLineEdit::Normal,
            "",
            &ok
    );

    FileCreationStatus status = FileCreationStatus::FAILED_UNKNOWN;

    if (ok && !dir_name.isEmpty())
    {
        std::string dir_name_str = dir_name.toStdString();
        status = m_song_gen_ptr->save_file(dir_name_str);
    }

    QString msg_box_str;
    switch(status){
        case FileCreationStatus::FAILED_UNKNOWN:{
            msg_box_str = "Song creation failed, try to reload the program";
            break;
        }
        case FileCreationStatus::FAILED_FILE_EMPTY:{
            msg_box_str = "Nothing to create, please generate the song first";
            break;
        }
        case FileCreationStatus::SUCCESS:{
            msg_box_str = "Song created! Check the application folder";
            break;
        }
    }


    QMessageBox msgBox(this);
    msgBox.setText(msg_box_str);
    msgBox.exec();
}

void App::resizeEvent(QResizeEvent *event){
    QDialog::resizeEvent(event);
    //ui->playSlider->setFixedWidth(ui->timelineTableWidget->columnWidth(1));
//    int columnWidth = ui->timelineTableWidget->columnWidth(1);
//    int columnX = ui->timelineTableWidget->columnViewportPosition(1);
//    ui->playSlider->setGeometry(columnX, ui->playSlider->y(), columnWidth, ui->playSlider->height());
}

void App::on_addChunkButton_clicked() {
    m_add_chunk();
}

void App::on_removeChunkButton_clicked() {
    m_pop_chunk();
}


