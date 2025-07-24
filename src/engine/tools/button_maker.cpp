#include "button_maker.h"
#include "engine/fs/fs_reading.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/param/actor_repository.h"
#include "engine/system/text_encoding.h"
#include "engine/param/actor/actor_loader.h"
#include "engine/fs/fs_characters.h"

ButtonMaker::ButtonMaker(QWidget *parent) : QWidget(parent)
{
  resize(960, 544);
  setStyleSheet(R"(
        QWidget { background-color: black; }
        QPushButton
        {
            background-color: #00E6AC;
            color: white;
            font-weight: bold;
            padding: 10px;
            border-radius: 10px;
        }
    )");


  m_GraphicsView = new DRGraphicsView(this);
  m_GraphicsView->resize(960, 544);

  m_GraphicsView->setStyleSheet("background: transparent");
  m_GraphicsView->setAttribute(Qt::WA_TranslucentBackground);
  m_GraphicsView->setBackgroundBrush(Qt::NoBrush);
  m_GraphicsView->scene()->setBackgroundBrush(Qt::NoBrush);

  m_CharacterSprite = new DRCharacterMovie(AOApplication::getInstance());
  m_CharacterSprite->set_size(QSizeF(960, 544));
  m_GraphicsView->scene()->addItem(m_CharacterSprite);
  m_CharacterSprite->show();

  m_Overlay = new ButtonMakerOverlay(this);
  m_Overlay->show();

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  QVBoxLayout *leftLayout = new QVBoxLayout();
  QVBoxLayout *rightLayout = new QVBoxLayout();


  QPushButton *generateButton = new QPushButton("Generate Button");
  QPushButton *underlayButton = new QPushButton("Add Underlay");
  QPushButton *overlayButton = new QPushButton("Add Overlay");
  QPushButton *alphaButton = new QPushButton("Add Alpha Mask");

  rightLayout->addWidget(underlayButton);
  rightLayout->addWidget(overlayButton);
  rightLayout->addWidget(alphaButton);
  rightLayout->addWidget(generateButton);

  connect(generateButton, &QPushButton::clicked, this, &ButtonMaker::onGenerateClicked);
  connect(underlayButton, &QPushButton::clicked, this, &ButtonMaker::onAddUnderlayClicked);
  connect(overlayButton, &QPushButton::clicked, this, &ButtonMaker::onAddOverlayClicked);
  connect(alphaButton, &QPushButton::clicked, this, &ButtonMaker::onAlphaClicked);

  mainLayout->addLayout(leftLayout);
  mainLayout->addStretch();
  mainLayout->addLayout(rightLayout);



}

void ButtonMaker::forceEmote(DREmote emote)
{
  m_CharacterSprite->play_idle(emote.character, emote.dialog);
  m_CharacterSprite->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));

  QStringList layers;
  for(const EmoteLayer &layer : emote.emoteOverlays)
  {
    if(engine::actor::user::layerState(layer.toggleName))
      layers.append(engine::system::encoding::text::EncodePacketContents({layer.spriteName, layer.spriteOrder, QString::number(layer.layerOffset.x()), QString::number(layer.layerOffset.y()), QString::number(layer.layerOffset.width()), QString::number(layer.layerOffset.height()), layer.offsetName}));
  }

  m_CharacterSprite->processOverlays(engine::system::encoding::text::EncodeBase64(layers), emote.character, emote.dialog, emote.outfitName);
  m_CharacterSprite->start(engine::actor::user::retrieve()->GetScalingMode(), (double)courtroom::sliders::getValue("scale_offset") / 1000.0f);
  return;
}

void ButtonMaker::SetEmote(DREmote emote)
{
  if(m_Emotes.count() == 0) return;

  if(emote.character != m_Emotes.at(0).character)
  {
    hide();
    return;
  }
  DREmote currentEmote = m_Emotes.at(m_EmoteIndex);
  if(emote.comment == currentEmote.comment && emote.outfitName == currentEmote.outfitName)
  {
    m_EmoteIndex = m_EmoteIndex;
    m_CharacterSprite->play_idle(currentEmote.character, currentEmote.dialog);
    m_CharacterSprite->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));

    QStringList layers;
    for(const EmoteLayer &layer : currentEmote.emoteOverlays)
    {
      if(engine::actor::user::layerState(layer.toggleName))
        layers.append(engine::system::encoding::text::EncodePacketContents({layer.spriteName, layer.spriteOrder, QString::number(layer.layerOffset.x()), QString::number(layer.layerOffset.y()), QString::number(layer.layerOffset.width()), QString::number(layer.layerOffset.height()), layer.offsetName}));
    }

    m_CharacterSprite->processOverlays(engine::system::encoding::text::EncodeBase64(layers), currentEmote.character, currentEmote.dialog, currentEmote.outfitName);
    m_CharacterSprite->start(engine::actor::user::retrieve()->GetScalingMode(), (double)courtroom::sliders::getValue("scale_offset") / 1000.0f);
    return;
  }

  for(int i = 0; i < m_Emotes.count(); i++)
  {
    DREmote checkEmote = m_Emotes.at(i);

    if(emote.comment == checkEmote.comment && emote.outfitName == checkEmote.outfitName)
    {
      m_EmoteIndex = i;
      m_CharacterSprite->play_idle(checkEmote.character, checkEmote.dialog);
      m_CharacterSprite->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));

      QStringList layers;
      for(const EmoteLayer &layer : checkEmote.emoteOverlays)
      {
        if(engine::actor::user::layerState(layer.toggleName))
          layers.append(engine::system::encoding::text::EncodePacketContents({layer.spriteName, layer.spriteOrder, QString::number(layer.layerOffset.x()), QString::number(layer.layerOffset.y()), QString::number(layer.layerOffset.width()), QString::number(layer.layerOffset.height()), layer.offsetName}));
      }

      m_CharacterSprite->processOverlays(engine::system::encoding::text::EncodeBase64(layers), checkEmote.character, checkEmote.dialog, checkEmote.outfitName);
      m_CharacterSprite->start(engine::actor::user::retrieve()->GetScalingMode(), (double)courtroom::sliders::getValue("scale_offset") / 1000.0f);
      return;
    }
  }

}

void ButtonMaker::SetCharacter(QString character)
{
  m_EmoteIndex = 0;
  m_Emotes.clear();

  m_Path = fs::characters::getFilePath(character, "char.json");
  if(FS::Checks::FileExists(m_Path))
  {
    m_IsJson = true;
    ActorDataReader jsonActor = ActorDataReader();
    jsonActor.LoadActor(character);
    jsonActor.SetOutfit("<All>");
    m_Emotes = jsonActor.GetEmotes();
  }
  else
  {
    m_IsJson = false;
    LegacyActorReader legacyActor = LegacyActorReader();
    legacyActor.LoadActor(character);
    m_Emotes = legacyActor.GetEmotes();
  }

  SetEmote(m_Emotes.at(m_EmoteIndex));
}

void ButtonMaker::onGenerateClicked()
{

  m_Overlay->setFocus();
  QRect cropRect(m_Overlay->m_rectPos, QSize(m_Overlay->m_rectSize, m_Overlay->m_rectSize));
  QSize outputSize = cropRect.size();

  if(!m_UnderlayImage.isNull()) outputSize = m_UnderlayImage.size();
  QImage finalOutput(outputSize, QImage::Format_ARGB32_Premultiplied);
  finalOutput.fill(Qt::transparent);
  QPainter finalPainter(&finalOutput);

  if (!m_UnderlayImage.isNull()) {
    QImage underlayScaled = m_UnderlayImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    finalPainter.drawImage(0, 0, underlayScaled);
  }

  QImage fullImage(m_GraphicsView->viewport()->size(), QImage::Format_ARGB32_Premultiplied);
  fullImage.fill(Qt::transparent);
  QPainter viewportPainter(&fullImage);
  m_GraphicsView->render(&viewportPainter);
  viewportPainter.end();

  //cropRect = cropRect.intersected(fullImage.rect());

  QImage cropppedSprite = fullImage.copy(cropRect);
  cropppedSprite = cropppedSprite.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  if (!m_AlphaMaskImage.isNull())
  {
    QImage maskScaled = m_AlphaMaskImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    cropppedSprite.setAlphaChannel(maskScaled);
  }

  finalPainter.drawImage(0, 0, cropppedSprite);

  if (!m_Overlay->m_OverlayImage.isNull())
  {
    QImage overlayScaled = m_Overlay->m_OverlayImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    finalPainter.drawImage(0, 0, overlayScaled);
  }

  finalPainter.end();

  QString buttonDirectory = "/emotions/button" + QString::number(m_EmoteIndex + 1) + "_off.png";
  if(m_IsJson)
  {
    buttonDirectory = "/outfits/" + m_Emotes.at(m_EmoteIndex).outfitName + "/emotions/" + m_Emotes.at(m_EmoteIndex).emoteName + ".png";
  }

  QString filePath = fs::characters::getFilePath(m_Emotes.at(m_EmoteIndex).character, buttonDirectory);
  if(FS::Checks::FileExists(filePath))
  {
    QMessageBox::StandardButton replaceResult;
    replaceResult = QMessageBox::question(nullptr, "Button Maker", "You are about to replace a button that already exists, continue?", QMessageBox::Yes|QMessageBox::No);
    if(replaceResult == QMessageBox::No)
    {
      return;
    }

  }
  if(!FS::Checks::DirectoryExists(QFileInfo(filePath).absolutePath()))
  {
    QDir().mkdir(QFileInfo(filePath).absolutePath());
  }
  finalOutput.save(filePath);

  if(m_EmoteIndex < (m_Emotes.count() - 1))
  {
    m_EmoteIndex++;
  }
  else
  {
    m_EmoteIndex = 0;
  }


  forceEmote(m_Emotes.at(m_EmoteIndex));
}

void ButtonMaker::onAddUnderlayClicked()
{
  m_UnderlayImage = LoadImageDialog();
  m_Overlay->setFocus();
}

void ButtonMaker::onAddOverlayClicked()
{
  m_Overlay->m_OverlayImage = LoadImageDialog();
  m_Overlay->setFocus();
}

void ButtonMaker::onAlphaClicked()
{
  m_AlphaMaskImage = LoadImageDialog();
  m_Overlay->setFocus();
}

QImage ButtonMaker::LoadImageDialog()
{
  QString filePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.bmp)");
  if (!filePath.isEmpty())
  {
    return QImage(filePath);
  }
  return QImage();
}

ButtonMakerOverlay::ButtonMakerOverlay(QWidget *parent) : QWidget(parent)
{
  setMouseTracking(true);
  resize(960, 544);
  m_rectPos = QPoint((width() / 2) - (m_rectSize / 2), 80);
  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

void ButtonMakerOverlay::keyPressEvent(QKeyEvent *event)
{

  switch(event->key())
  {
    case Qt::Key_Up:
      m_rectPos.setY(m_rectPos.y() - 1);
      break;
    case Qt::Key_Down:
      m_rectPos.setY(m_rectPos.y() + 1);
      break;
    case Qt::Key_Right:
      m_rectPos.setX(m_rectPos.x() + 1);
      break;
    case Qt::Key_Left:
      m_rectPos.setX(m_rectPos.x() - 1);
      break;
    case Qt::Key_G:
      m_renderGuides = m_renderGuides == false;
      break;

  }

    update();
}

void ButtonMakerOverlay::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setPen(Qt::white);
  QRect rect(m_rectPos, QSize(m_rectSize, m_rectSize));
  painter.drawRect(rect);

  if(m_renderGuides)
  {
    QPoint center = rect.center();
    painter.setPen(QPen(Qt::red, 1, Qt::DashLine));

    painter.drawLine(QPoint(rect.left(), center.y()), QPoint(rect.right(), center.y()));
    painter.drawLine(QPoint(center.x(), rect.top()), QPoint(center.x(), rect.bottom()));
  }
  if(m_OverlayImage.isNull()) return;
  painter.drawImage(rect, m_OverlayImage);

}

void ButtonMakerOverlay::mousePressEvent(QMouseEvent *event)
{
  setFocus();
  QRect rect(m_rectPos, QSize(m_rectSize, m_rectSize));
  if (rect.contains(event->pos())) {
    m_dragging = true;
    m_dragOffset = event->pos() - m_rectPos;
  }
}

void ButtonMakerOverlay::mouseMoveEvent(QMouseEvent *event)
{
  if (!m_dragging) return;

  m_rectPos = event->pos() - m_dragOffset;
  update();
}

void ButtonMakerOverlay::mouseReleaseEvent(QMouseEvent *)
{
  m_dragging = false;
}

void ButtonMakerOverlay::wheelEvent(QWheelEvent *event)
{
  int delta = event->angleDelta().y() / 8;
  m_rectSize += delta / 3;
  m_rectSize = std::max(40, std::min(200, m_rectSize));
  update();
}
