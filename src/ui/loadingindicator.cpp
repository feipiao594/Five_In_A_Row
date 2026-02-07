#include "loadingindicator.h"

#include <QPainter>

void LoadingIndicator::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  const int w = width();
  const int h = height();
  const QPointF c(w / 2.0, h / 2.0);
  const double radius = qMin(w, h) / 2.0 - 1.0;

  p.translate(c);

  for (int i = 0; i < 12; i++) {
    const int idx = (frame_ + i) % 12;
    const int alpha = 20 + (11 - i) * 18; // 20..218

    p.save();
    p.rotate(idx * 30.0);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(80, 80, 80, alpha));

    const QRectF r(radius * 0.45, -radius * 0.12, radius * 0.35,
                   radius * 0.24);
    p.drawRoundedRect(r, radius * 0.12, radius * 0.12);
    p.restore();
  }
}

