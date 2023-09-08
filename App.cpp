//
// Created by mi on 05.08.2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_App.h" resolved


#include "App.h"
#include "ui_App.h"

#include "../SongGen/SongGen.h"
#include "../SongGen/SongGenEnumsToString.h"
//#include "../SongGen/cxxmidi/player/player_sync_copy.hpp"

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
        slider->setSliderPosition(max_pos * mult);
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
}

App::~App() {
    m_song_gen_ptr->pause_playing();
    m_song_gen_ptr->terminate_playback();
    m_play_thread.join();
    delete ui;
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
        if(progression == "Random") progression = "";
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

    int row = ui->createTableWidget->rowCount();
    ui->createTableWidget->insertRow(row);

    auto nameLineEdit = new QLineEdit;
    QRegularExpression rx("[\\x00-\\x7F]*");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    nameLineEdit->setValidator(validator);
    QString track_name = "track_" + QString::fromStdString(std::to_string(row+1));
    nameLineEdit->setText(track_name);
    auto& track = m_song_gen_ptr->get_track((size_t)row);
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

    connect(rangeComboBox, &QComboBox::currentIndexChanged, this, [this, rangeComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto num = rangeComboBox->currentText().toUInt();
        track.set_range_in_octaves(num);
    });


    auto progrStyleComboBox = new QComboBox;
    for(auto&[str, en] : progression_style_vec)
        progrStyleComboBox->addItem(QString::fromStdString(str));
    ui->createTableWidget->setCellWidget(row, 3, progrStyleComboBox);

    connect(progrStyleComboBox, &QComboBox::currentIndexChanged, this, [this, progrStyleComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = progrStyleComboBox->currentText().toStdString();
        auto it = std::find_if(
                progression_style_vec.begin(),
                progression_style_vec.end(),
                [&](auto& pair){ return pair.first == str; }
        );

        auto&[k,progrStyle] = *it;
        track.set_progression_style(progrStyle);
    });


    auto noteLenBiasComboBox = new QComboBox;
    for(auto&[str, en] : note_len_bias_vec)
        noteLenBiasComboBox->addItem(QString::fromStdString(str));
    ui->createTableWidget->setCellWidget(row, 4, noteLenBiasComboBox);

    connect(noteLenBiasComboBox, &QComboBox::currentIndexChanged, this, [this, noteLenBiasComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = noteLenBiasComboBox->currentText().toStdString();
        auto it = std::find_if(
                note_len_bias_vec.begin(),
                note_len_bias_vec.end(),
                [&](auto& pair){ return pair.first == str; }
        );

        auto&[k,noteLenBias] = *it;
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

    connect(customNoteLenComboBox, &QComboBox::currentIndexChanged, this, [this, customNoteLenComboBox, row]{
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

}

void App::on_removeRowButton_clicked() {
    int row = ui->createTableWidget->rowCount();
    ui->createTableWidget->removeRow(row - 1);
    m_song_gen_ptr->pop_track();
}

void App::on_createButton_clicked() {
    //m_song_gen_ptr->print_tracks_info();
    m_song_gen_ptr->create();
    m_song_gen_ptr->mute_playing();
    m_song_gen_ptr->pause_playing();
    m_populate_timeline();
    m_song_gen_ptr->set_file_to_play();
}

void App::m_populate_timeline(){
    const auto& file = m_song_gen_ptr->get_file();
    const auto size = file.size();


    auto table = ui->timelineTableWidget;
    auto header = (SliderHeader*)table->horizontalHeader();
    auto slider = header->getSlider();

    auto slider_width = slider->width();


    int row = table->rowCount();
    while(row){
        table->removeRow(row - 1);
        row = table->rowCount();
    }

    auto column_index = 1;
    table->setColumnWidth(column_index, slider_width);

    std::cout << "sliderwidth: " << slider_width << '\n';

    for(auto i = 0; i < size; i++){

        auto& track = file[i];

        // create row
        table->insertRow(i);

        int buffers = 16;

        auto* widget = new QWidget;
        auto filter = new ButtonEventFilter(widget);

        widget->setContentsMargins(0,0,0,0);

        auto* layout = new QHBoxLayout(widget);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);


        for(int j = 0; j < buffers; j++){

            auto button = new QPushButton("");
            auto cmh = new ContextMenuHandler(button);

            button->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(
                    button,
                    &QPushButton::customContextMenuRequested,
                    cmh,
                    &ContextMenuHandler::showButtonContextMenu
            );

            button->installEventFilter(filter);
            button->setAcceptDrops(true);

            layout->addWidget(button);

        }
        widget->setLayout(layout);
        table->setCellWidget(i, column_index, widget);

    }
}

void App::on_sliderPressed(){
    std::cout << "slider pressed!\n";
    //m_song_gen_ptr->mute_playing();
    //m_song_gen_ptr->pause_playing();
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

    //bool is_paused = m_song_gen_ptr->is_playback_paused();

    m_song_gen_ptr->mute_playing();
    m_song_gen_ptr->pause_playing();

    auto header = (SliderHeader*)ui->timelineTableWidget->horizontalHeader();
    auto slider = header->getSlider();

    m_song_gen_ptr->set_play_position(
            slider->minimum(),
            slider->maximum(),
            (uint32_t)value);

    //if(!is_paused)
    //m_song_gen_ptr->resume_playing();

    std::cout << "slider value changed: " << value << '\n';
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
    m_song_gen_ptr->mute_playing();
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


