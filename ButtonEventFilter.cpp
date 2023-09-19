#include "ButtonEventFilter.h"

#include "Cell.h"

ButtonEventFilter::ButtonEventFilter(QWidget *parent_widget,
                                     QObject *parent,
                                     SongGen *sg)
        : QObject(parent), m_parent_widget(parent_widget),
          m_dragStartPosition(), m_song_gen_ptr(sg) {

}


bool ButtonEventFilter::eventFilter(QObject *watched, QEvent *event) {
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            handleMouseButtonPress(watched, event);
            break;
        case QEvent::MouseMove:
            handleMouseMove(watched, event);
            break;
        case QEvent::MouseButtonRelease:
            handleMouseButtonRelease(watched, event);
            break;
        default:
            break;
    }
    return
            QObject::eventFilter(watched, event);
}

void ButtonEventFilter::handleMouseButtonPress(QObject *watched, QEvent *event) {
    auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    if (mouseEvent->button() == Qt::LeftButton) {
        m_dragStartPosition = mouseEvent->pos();
        m_draggedWidget = qobject_cast<Cell *>(watched);
        m_initialDraggedWidgetPos = m_draggedWidget->pos();
        std::cout << "dragging..\n";
    }
}
void ButtonEventFilter::handleMouseMove(QObject* watched, QEvent* event)
{
    auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!(mouseEvent->buttons() & Qt::LeftButton)) {
        return;
    }
    if (!m_draggedWidget) {
        return;
    }
    updateDraggedWidgetPosition(mouseEvent);
    Cell* targetWidget = findTargetWidget();
    swapDraggedAndTargetWidgets(targetWidget);
}

void ButtonEventFilter::handleMouseButtonRelease(QObject* watched, QEvent* event)
{
    std::cout << "released..\n";

    if(m_draggedWidget){
        m_draggedWidget->move(m_initialDraggedWidgetPos);

        m_draggedWidget = nullptr;
    }
}

void ButtonEventFilter::updateDraggedWidgetPosition(QMouseEvent* mouseEvent)
{
    auto btn_pos = m_draggedWidget->pos();
    auto new_x = btn_pos.x() + (mouseEvent->pos().x() - m_dragStartPosition.x());

    new_x = qMax(new_x, 0);
    new_x = qMin(new_x, m_parent_widget->width() - m_draggedWidget->width());

    btn_pos.setX(new_x);
    m_draggedWidget->move(btn_pos);
}

Cell* ButtonEventFilter::findTargetWidget()
{
    if(!m_draggedWidget)
        return nullptr;

    Cell* targetWidget = nullptr;

    auto layout = m_parent_widget->layout();
    auto size = layout->count();
    auto main_box = m_draggedWidget->geometry();

    for(int i = 0; i < size; i++){

        auto curr_btn = (Cell*)layout->itemAt(i)->widget();

        if(curr_btn == m_draggedWidget)
            continue;

        auto curr_box = curr_btn->geometry();

        bool collision = main_box.intersects(curr_box);

        if (collision) {
            targetWidget = curr_btn;
            break;
        }

    }
    return targetWidget;
}

void ButtonEventFilter::swapDraggedAndTargetWidgets(Cell* targetWidget)
{
    if (!targetWidget || !m_draggedWidget)
        return;


    bool shouldSwap = false;
    QPoint pos = m_draggedWidget->pos();
    if (m_initialDraggedWidgetPos.x() < pos.x()) { // moving right
        shouldSwap = pos.x() >= targetWidget->pos().x() - (targetWidget->width() * 0.3);
    } else { // moving left
        shouldSwap = pos.x() <= targetWidget->pos().x() + (targetWidget->width() * 0.3);
    }

    if(!shouldSwap)
        return;

    auto layout = qobject_cast<QBoxLayout*>(m_parent_widget->layout());

    int targetPos = layout->indexOf(m_draggedWidget);
    int sourcePos = layout->indexOf(targetWidget);

    m_initialDraggedWidgetPos = targetWidget->pos();

    auto& buffers = m_draggedWidget->get_track_ptr()->get_buffers();

    auto i1 = m_draggedWidget->get_buff_index();
    auto i2 = targetWidget->get_buff_index();

    m_draggedWidget->set_buff_index(i2);
    targetWidget->set_buff_index(i1);


    std::swap(buffers[i1], buffers[i2]);


    layout->insertWidget(targetPos, targetWidget);
    layout->insertWidget(sourcePos, m_draggedWidget);

    if(!m_song_gen_ptr){
        std::cout << "error at ButtonEventFilter::swapDraggedAndTargetWidgets - "
                     "song_gen_ptr is null\n";
        return;
    }

    m_song_gen_ptr->set_file_to_play();

}