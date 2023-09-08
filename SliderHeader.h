//
// Created by mi on 13.08.2023.
//

#ifndef SONGGENGUI_SLIDERHEADER_H
#define SONGGENGUI_SLIDERHEADER_H
#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionSlider>

#include "CustomSlider.h"

class SliderHeader : public QHeaderView {
Q_OBJECT
public:
    explicit SliderHeader(Qt::Orientation orientation, QWidget *parent = nullptr)
            : QHeaderView(orientation, parent), m_slider_ptr(new CustomSlider(this)) {

        m_slider_ptr->setOrientation(Qt::Horizontal);
        m_slider_ptr->setRange(0, 999);


}

    CustomSlider* getSlider(){
        return m_slider_ptr;
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override {
        painter->save();
        QHeaderView::paintSection(painter, rect, logicalIndex);
        painter->restore();

        if (logicalIndex == 1) {

            //std::cout << "drawing slider.." << '\n';

            m_slider_ptr->move(rect.x(), rect.y());

            m_slider_ptr->resize(rect.size());
            QPixmap pixmap(rect.size());
            m_slider_ptr->render(&pixmap);
            painter->drawPixmap(rect.topLeft(), pixmap);

//            auto table = qobject_cast<QTableWidget *>(parentWidget());
//            if (table) {
//                m_slider_ptr->drawLineOnTable(table, painter);
//            }
        }
    }

private:
    CustomSlider *m_slider_ptr;
};

#endif //SONGGENGUI_SLIDERHEADER_H
