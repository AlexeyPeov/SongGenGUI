//
// Created by mi on 05.08.2023.
//
#ifndef SONGGENGUI_APP_H
#define SONGGENGUI_APP_H

#include <memory>

#include <QDialog>
#include <QRegularExpression>


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
    void on_styleComboBox_currentIndexChanged(int index);

    void on_barAmountLineEdit_textEdited(const QString& str);

    void on_addRowButton_clicked();

    void on_removeRowButton_clicked();

    void on_createButton_clicked();

private:
    Ui::App *ui;
    std::unique_ptr<SongGen> m_song_gen_ptr;

};


#endif //SONGGENGUI_APP_H
