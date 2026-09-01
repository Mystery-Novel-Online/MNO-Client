#ifndef CROSSFADELABEL_H
#define CROSSFADELABEL_H

class CrossfadeLabel : public RPLabel
{
  Q_OBJECT
public:
  CrossfadeLabel(AOApplication* aoApp, QWidget* parent);

  void assignRenderTarget(DRGraphicsView* viewport);
  void setFadeDuration(int durationMs) { m_fadeDuration = durationMs; }

  void render();
  void animate();

private:
  DRGraphicsView* m_renderTarget = nullptr;
  int m_fadeDuration = 200;
};

#endif // CROSSFADELABEL_H
