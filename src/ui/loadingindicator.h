#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QTimer>
#include <QWidget>

class LoadingIndicator final : public QWidget {
public:
  explicit LoadingIndicator(QWidget* parent = nullptr, int size = 18)
      : QWidget(parent) {
    setFixedSize(size, size);
    timer_.setInterval(60);
    connect(&timer_, &QTimer::timeout, this, [this]() {
      frame_ = (frame_ + 1) % 12;
      update();
    });
    setVisible(false);
  }

  void start() {
    if (timer_.isActive())
      return;
    frame_ = 0;
    setVisible(true);
    timer_.start();
  }

  void stop() {
    timer_.stop();
    setVisible(false);
  }

protected:
  void paintEvent(QPaintEvent*) override;

private:
  QTimer timer_;
  int frame_ = 0;
};

#endif
