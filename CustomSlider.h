//
// Created by mi on 13.08.2023.
//

#ifndef SONGGENGUI_CUSTOMSLIDER_H
#define SONGGENGUI_CUSTOMSLIDER_H



#include <QSlider>
#include <QStyleOptionSlider>
#include <QPainter>
#include <QTableWidget>

class CustomSlider : public QSlider
{
public:
    CustomSlider(QWidget *parent = nullptr) : QSlider(parent) {

    }

    void initStyleOptionPublic(QStyleOptionSlider *option) const {
        initStyleOption(option);
    }

    void drawLineOnTable(QTableWidget* table, QPainter* painter){
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        QPoint handleCenter = handleRect.center();

        // draw line from the handle center to the table bottom
        painter->setViewport(table->viewport()->rect());
        //QPainter painter(table->viewport());
        QPoint bottomCenter(handleCenter.x(), table->viewport()->height());
        painter->drawLine(handleCenter, bottomCenter);
    }


protected:
    void paintEvent(QPaintEvent *event) override
    {
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QPainter painter(this);
        style()->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);

        QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        QPoint handleTopLeft = handleRect.topLeft();
        QPoint handleBottomRight = handleRect.bottomRight();

        QPoint handleBottomLeft = handleRect.bottomLeft();

        auto bottom_center_x = (handleBottomRight.x() + handleBottomLeft.x()) / 2;

        QPoint bottom_center = {bottom_center_x, handleBottomRight.y()};

        QPoint handleTopRight = handleRect.topRight();

        QPoint handleCenter = handleRect.center();

        QPolygon triangle;
        triangle << handleTopRight << bottom_center << handleTopLeft;
        painter.setBrush(Qt::black);
        painter.drawPolygon(triangle);

        //std::cout << "sl x: " << pos().x() << " y: " << pos().y() << '\n';

        //painter.drawLine(handleCenter, bottomCenter);
    }

    void mousePressEvent(QMouseEvent *event) override {
        // Check if left button was clicked
        if (event->button() == Qt::LeftButton) {
            // Get mouse position relative to the slider
            int mousePos = event->pos().x();

            // Map mouse position to slider range
            double posRatio = static_cast<double>(mousePos) / width();
            int sliderRange = maximum() - minimum();
            int sliderPos = posRatio * sliderRange + minimum();

            // Set the slider value
            setValue(sliderPos);
        }

        // Call base class implementation
        QSlider::mousePressEvent(event);
    }

};

#endif //SONGGENGUI_CUSTOMSLIDER_H
