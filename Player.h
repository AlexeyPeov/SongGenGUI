//
// Created by mi on 07.08.2023.
//

#ifndef SONGGENGUI_PLAYER_H
#define SONGGENGUI_PLAYER_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QAudioSink>

class Player : public QObject {
Q_OBJECT

public:
    Player(QObject *parent = nullptr) : QObject(parent) {
        // Create a new thread for the audio sink
        thread_ = new QThread(this);
        thread_->start();

        // Create an audio sink and move it to the new thread
        audio_sink_ = new QAudioSink;
        audio_sink_->moveToThread(thread_);
    }

    ~Player() {
        // Stop the audio sink and delete it
        audio_sink_->stop();
        delete audio_sink_;

        // Quit the thread and wait for it to finish
        thread_->quit();
        thread_->wait();
    }

public slots:
    void load(const QString &path) {
        // Load the MIDI file data into a buffer
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            buffer_.setData(file.readAll());
            buffer_.open(QIODevice::ReadOnly);
            file.close();
        }
    }

    void play() {
        // Start playing the MIDI data from the buffer
        audio_sink_->start(&buffer_);
    }

    void pause() {
        // Suspend playback of the MIDI data
        audio_sink_->suspend();
    }

    void stop() {
        // Stop playback of the MIDI data and reset the buffer position
        audio_sink_->stop();
        buffer_.seek(0);
    }

    void set_pos(qint64 position) {
        // Set the position of the buffer
        buffer_.seek(position);
    }

private:
    QThread *thread_;
    QAudioSink *audio_sink_;
    QBuffer buffer_;
};

#endif //SONGGENGUI_PLAYER_H
