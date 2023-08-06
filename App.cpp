//
// Created by mi on 05.08.2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_App.h" resolved

#include "App.h"
#include "ui_App.h"
#include "../SongGen/SongGen.h"
#include "../SongGen/SongGenEnumsToString.h"

App::App(QWidget *parent) :
        QDialog(parent), ui(new Ui::App) {
    ui->setupUi(this);
    m_song_gen_ptr = std::make_unique<SongGen>();
    m_song_gen_ptr->set_delta_time(converters::microseconds_to_delta_time(500000,500000,500));

    for (auto &[str, en]: style_map)
        ui->styleComboBox->addItem(QString::fromStdString(str));

    for (auto &[str, en]: keys_map)
        ui->keyComboBox->addItem(QString::fromStdString(str));

    for (auto &[str, en]: mood_map)
        ui->moodComboBox->addItem(QString::fromStdString(str));

}

App::~App() {
    delete ui;
}

void App::on_styleComboBox_currentIndexChanged(int index) {
    auto str = ui->styleComboBox->currentText().toStdString();
    auto style = style_map.at(str);
    m_song_gen_ptr->set_style(style);
}

void App::on_barAmountLineEdit_textEdited(const QString &str) {
    Q_UNUSED(str)
    bool ok = false;
    uint32_t v = ui->barAmountLineEdit->text().toUInt(&ok);

    if (!ok)
        ui->barAmountLineEdit->setText("");
    else
        m_song_gen_ptr->set_bar_amount(v);

}

void App::on_addRowButton_clicked() {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    auto nameLineEdit = new QLineEdit;
    QRegularExpression rx("[\\x00-\\x7F]*");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    nameLineEdit->setValidator(validator);
    ui->tableWidget->setCellWidget(row, 0, nameLineEdit);

    connect(nameLineEdit, &QLineEdit::textEdited, this, [this, nameLineEdit, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = nameLineEdit->text().toStdString();
        track.set_name(str);
    });


    auto startOctaveComboBox = new QComboBox;
    for(int i = 1; i < 9; i++)
        startOctaveComboBox->addItem(QString(std::to_string(i).c_str()));

    ui->tableWidget->setCellWidget(row, 1, startOctaveComboBox);

    connect(startOctaveComboBox, &QComboBox::currentIndexChanged, this, [this, startOctaveComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto num = startOctaveComboBox->currentText().toUInt();
        track.set_start_octave(num);
    });


    auto rangeComboBox = new QComboBox;
    for(int i = 1; i < 6; i++)
        rangeComboBox->addItem(QString(std::to_string(i).c_str()));
    ui->tableWidget->setCellWidget(row, 2, rangeComboBox);

    connect(rangeComboBox, &QComboBox::currentIndexChanged, this, [this, rangeComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto num = rangeComboBox->currentText().toUInt();
        track.set_range_in_octaves(num);
    });


    auto progrStyleComboBox = new QComboBox;
    for(auto&[str, en] : progression_style_map)
        progrStyleComboBox->addItem(QString::fromStdString(str));
    ui->tableWidget->setCellWidget(row, 3, progrStyleComboBox);

    connect(progrStyleComboBox, &QComboBox::currentIndexChanged, this, [this, progrStyleComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = progrStyleComboBox->currentText().toStdString();
        auto progrStyle = progression_style_map.at(str);
        track.set_progression_style(progrStyle);
    });


    auto noteLenBiasComboBox = new QComboBox;
    for(auto&[str, en] : note_len_bias_map)
        noteLenBiasComboBox->addItem(QString::fromStdString(str));
    ui->tableWidget->setCellWidget(row, 4, noteLenBiasComboBox);

    connect(noteLenBiasComboBox, &QComboBox::currentIndexChanged, this, [this, noteLenBiasComboBox, row]{
        auto& track = m_song_gen_ptr->get_track((size_t)row);
        auto str = noteLenBiasComboBox->currentText().toStdString();
        auto noteLenBias = note_len_bias_map.at(str);
        track.set_note_len_bias(noteLenBias);
    });

    auto customNoteLenComboBox = new QComboBox;
    customNoteLenComboBox->addItem("No");
    customNoteLenComboBox->addItem("Sixteenth");
    customNoteLenComboBox->addItem("Eigth");
    customNoteLenComboBox->addItem("Quarter");
    customNoteLenComboBox->addItem("Half");
    customNoteLenComboBox->addItem("Whole");
    ui->tableWidget->setCellWidget(row, 5, customNoteLenComboBox);

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
    // song gen

    m_song_gen_ptr->add_track();

}

void App::on_removeRowButton_clicked() {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->removeRow(row - 1);
    m_song_gen_ptr->pop_track();
}

void App::on_createButton_clicked() {
    m_song_gen_ptr->create("song.mid");
}


