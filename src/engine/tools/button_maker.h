#ifndef BUTTON_MAKER_H
#define BUTTON_MAKER_H

class DRGraphicsView;
class DRCharacterMovie;

class ButtonMakerOverlay : public QWidget
{
  Q_OBJECT
public:
  ButtonMakerOverlay(QWidget *parent = nullptr);

public:
  QPoint m_rectPos {0, 0};
  int m_rectSize = 120;
  bool m_dragging = false;
  QPoint m_dragOffset;
  bool m_renderGuides = false;
  QImage m_OverlayImage;

  QMap<int, QPoint> m_presetPositions = {};
  QMap<int, int> m_presetScales = {};

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
};

class ButtonMaker : public QWidget
{
  Q_OBJECT
public:
  ButtonMaker(QWidget *parent = nullptr);
  int findEmote(const ActorEmote &emote) const;
  void displayEmote(const ActorEmote& emote);
  void nextEmote();

  void SetEmote(const ActorEmote& emote);
  void SetCharacter(QString character);

private slots:
  void onGenerateClicked();
  void onPreviewGenClicked();
  void onAddUnderlayClicked();
  void onAddOverlayClicked();
  void onAlphaClicked();
private:
  QImage loadImage(QImage &target);
  QImage LoadImageDialog();
  QImage captureViewport() const;

  bool saveImage(const QImage& image, const QString& path, const QString& imageType);


private:

  QVector<ActorEmote> m_Emotes = {};
  int m_EmoteIndex = 0;

  bool m_IsJson = false;
  QString m_Path = "";

  DRCharacterMovie *m_CharacterSprite = nullptr;
  DRGraphicsView *m_GraphicsView = nullptr;
  ButtonMakerOverlay *m_Overlay = nullptr;

  QImage m_UnderlayImage;
  QImage m_AlphaMaskImage;


};



#endif // BUTTON_MAKER_H
