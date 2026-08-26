#include "button_maker.h"
#include "engine/fs/fs_reading.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/param/actor_repository.h"
#include "engine/system/text_encoding.h"
#include "engine/param/actor/actor_loader.h"
#include "engine/fs/fs_characters.h"

#include <rolechat/actor/JsonActorData.h>

ButtonMaker::ButtonMaker(QWidget *parent) : QWidget(parent)
{
  resize(960, 540);
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
  m_GraphicsView->resize(960, 540);

  m_GraphicsView->setStyleSheet("background: transparent");
  m_GraphicsView->setAttribute(Qt::WA_TranslucentBackground);
  m_GraphicsView->setBackgroundBrush(Qt::NoBrush);
  m_GraphicsView->scene()->setBackgroundBrush(Qt::NoBrush);

  m_CharacterSprite = new DRCharacterMovie(AOApplication::getInstance());
  m_CharacterSprite->set_size(QSizeF(960, 540));
  m_GraphicsView->scene()->addItem(m_CharacterSprite);
  m_CharacterSprite->show();

  u_overlayDisplay = new ButtonMakerOverlay(this);
  u_overlayDisplay->show();

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  QVBoxLayout *leftLayout = new QVBoxLayout();
  QVBoxLayout *rightLayout = new QVBoxLayout();


  QPushButton *generateButton = new QPushButton("Generate Button");
  QPushButton *generatePreviewButton = new QPushButton("Generate Preview");
  QPushButton *underlayButton = new QPushButton("Add Underlay");
  QPushButton *overlayButton = new QPushButton("Add Overlay");
  QPushButton *alphaButton = new QPushButton("Add Alpha Mask");

  rightLayout->addWidget(underlayButton);
  rightLayout->addWidget(overlayButton);
  rightLayout->addWidget(alphaButton);
  rightLayout->addWidget(generatePreviewButton);
  rightLayout->addWidget(generateButton);

  connect(generateButton, &QPushButton::clicked, this, &ButtonMaker::onGenerateClicked);
  connect(generatePreviewButton, &QPushButton::clicked, this, &ButtonMaker::onPreviewGenClicked);
  connect(underlayButton, &QPushButton::clicked, this, &ButtonMaker::onAddUnderlayClicked);
  connect(overlayButton, &QPushButton::clicked, this, &ButtonMaker::onAddOverlayClicked);
  connect(alphaButton, &QPushButton::clicked, this, &ButtonMaker::onAlphaClicked);

  mainLayout->addLayout(leftLayout);
  mainLayout->addStretch();
  mainLayout->addLayout(rightLayout);



}

int ButtonMaker::findEmote(const ActorEmote &emote) const
{
  for(int i = 0; i < m_Emotes.size(); ++i)
  {
    if(m_Emotes[i].comment == emote.comment &&
        m_Emotes[i].outfitName == emote.outfitName)
      return i;
  }

  return -1;
}

void ButtonMaker::displayEmote(const ActorEmote &emote)
{
  m_CharacterSprite->play_idle(QString::fromStdString(emote.character), QString::fromStdString(emote.dialog));
  m_CharacterSprite->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));

  QStringList layers;
  for(const ActorLayer &layer : emote.emoteOverlays)
  {
    if(engine::actor::user::layerState(layer.toggleName))
      layers.append(engine::system::encoding::text::EncodePacketContents({QString::fromStdString(layer.spriteName), QString::fromStdString(layer.spriteOrder), QString::number(layer.layerOffset.x), QString::number(layer.layerOffset.y), QString::number(layer.layerOffset.width), QString::number(layer.layerOffset.height), QString::fromStdString(layer.offsetName)}));
  }

  m_CharacterSprite->processOverlays(engine::system::encoding::text::EncodeBase64(layers), QString::fromStdString(emote.character), QString::fromStdString(emote.dialog), QString::fromStdString(emote.outfitName));
  m_CharacterSprite->start(engine::actor::user::retrieve()->scalingMode(), (double)courtroom::sliders::getValue("scale_offset") / 1000.0f);
}

void ButtonMaker::nextEmote()
{
  m_EmoteIndex = (m_EmoteIndex + 1) % m_Emotes.size();
  displayEmote(m_Emotes[m_EmoteIndex]);
}


void ButtonMaker::SetEmote(const ActorEmote& emote)
{
  if(m_Emotes.empty())
    return;

  if(emote.character != m_Emotes.at(0).character)
  {
    hide();
    return;
  }

  int index = findEmote(emote);

  if(index == -1)
    return;

  m_EmoteIndex = index;
  displayEmote(m_Emotes[index]);
}

void ButtonMaker::SetCharacter(QString character)
{
  m_EmoteIndex = 0;
  m_Emotes.clear();

  m_Path = engine::fs::characters::getFilePath(character, "char.json");
  std::vector<ActorEmote> emotes;

  if(FS::Checks::FileExists(m_Path))
  {
    m_IsJson = true;
    rolechat::actor::JsonActorData jsonActor = rolechat::actor::JsonActorData();
    jsonActor.load(character.toStdString(), engine::fs::characters::getDirectoryPath(character).toStdString());
    jsonActor.switchOutfit("<All>");
    emotes = jsonActor.emotes();
  }
  else
  {
    m_IsJson = false;
    LegacyActorReader legacyActor = LegacyActorReader();
    legacyActor.load(character.toStdString(), engine::fs::characters::getDirectoryPath(character).toStdString());
    emotes = legacyActor.emotes();
  }

  m_Emotes.clear();
  for(const ActorEmote& emote : emotes)
    m_Emotes << emote;

  SetEmote(m_Emotes.at(m_EmoteIndex));
}

void ButtonMaker::onGenerateClicked()
{

  u_overlayDisplay->setFocus();
  QRect cropRect(u_overlayDisplay->m_rectPos, QSize(u_overlayDisplay->m_rectSize, u_overlayDisplay->m_rectSize));
  QSize outputSize = cropRect.size();

  if(outputSize.width() > 82)
    outputSize = {82, 82};

  if(!m_UnderlayImage.isNull()) outputSize = m_UnderlayImage.size();

  QImage finalOutput = drawButton();
  u_overlayDisplay->m_buttonPreview = finalOutput;

  QString buttonDirectory = m_IsJson ? QString::fromStdString("/outfits/" + m_Emotes.at(m_EmoteIndex).outfitName + "/emotions/" + m_Emotes.at(m_EmoteIndex).emoteName + ".webp")
                                     : "/emotions/button" + QString::number(m_EmoteIndex + 1) + "_off.webp";

  if(!saveImage(finalOutput, buttonDirectory, "button"))
    return;

  nextEmote();
}

void ButtonMaker::onPreviewGenClicked()
{

  u_overlayDisplay->setFocus();

  QImage fullImage = captureViewport();
  if(!saveImage(fullImage, "/previews/" + QString::fromStdString(m_Emotes.at(m_EmoteIndex).emoteName) + ".png", "preview"))
    return;

  nextEmote();
}

void ButtonMaker::onAddUnderlayClicked()
{
  loadImage(m_UnderlayImage);
}

void ButtonMaker::onAddOverlayClicked()
{
  loadImage(u_overlayDisplay->m_OverlayImage);
}

void ButtonMaker::onAlphaClicked()
{
  loadImage(m_AlphaMaskImage);
}

void ButtonMaker::loadImage(QImage &target)
{
  target = LoadImageDialog();
  u_overlayDisplay->setFocus();
}

QImage ButtonMaker::LoadImageDialog()
{
  QString filePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.bmp)");
  if(!filePath.isEmpty())
  {
    return QImage(filePath);
  }
  return QImage();
}

QImage ButtonMaker::captureViewport() const
{
  QImage image(m_GraphicsView->viewport()->size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  m_GraphicsView->render(&painter);

  return image;
}

QImage ButtonMaker::drawButton() const
{
  QRect cropRect(u_overlayDisplay->m_rectPos, QSize(u_overlayDisplay->m_rectSize, u_overlayDisplay->m_rectSize));
  QSize outputSize = cropRect.size();

  if(outputSize.width() > 82)
    outputSize = {82, 82};

  if(!m_UnderlayImage.isNull()) outputSize = m_UnderlayImage.size();
  QImage finalOutput(outputSize, QImage::Format_ARGB32_Premultiplied);
  finalOutput.fill(Qt::transparent);
  QPainter finalPainter(&finalOutput);

  if(!m_UnderlayImage.isNull()) {
    QImage underlayScaled = m_UnderlayImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    finalPainter.drawImage(0, 0, underlayScaled);
  }

  QImage fullImage = captureViewport();

  QImage cropppedSprite = fullImage.copy(cropRect);
  cropppedSprite = cropppedSprite.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  if(!m_AlphaMaskImage.isNull())
  {
    QImage maskScaled = m_AlphaMaskImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    cropppedSprite.setAlphaChannel(maskScaled);
  }

  finalPainter.drawImage(0, 0, cropppedSprite);

  if(!u_overlayDisplay->m_OverlayImage.isNull())
  {
    QImage overlayScaled = u_overlayDisplay->m_OverlayImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    finalPainter.drawImage(0, 0, overlayScaled);
  }

  return finalOutput;
}

bool ButtonMaker::saveImage(const QImage &image, const QString &path, const QString& imageType)
{
  QString filePath = engine::fs::characters::getFilePath(QString::fromStdString(m_Emotes.at(m_EmoteIndex).character), path);

  if(FS::Checks::FileExists(filePath))
  {
    QMessageBox::StandardButton replaceResult;
    replaceResult = QMessageBox::question(nullptr, "Button Maker", "You are about to replace a " + imageType + " that already exists, continue?", QMessageBox::Yes|QMessageBox::No);

    if(replaceResult == QMessageBox::No)
      return false;
  }

  if(!FS::Checks::DirectoryExists(QFileInfo(filePath).absolutePath()))
    QDir().mkdir(QFileInfo(filePath).absolutePath());

  if(imageType == "button")
    image.save(filePath, "WEBP", 100);
  else
    image.save(filePath);
  return true;
}

ButtonMakerOverlay::ButtonMakerOverlay(QWidget *parent) : QWidget(parent)
{
  setMouseTracking(true);
  resize(960, 540);
  m_rectPos = QPoint((width() / 2) - (m_rectSize / 2), 80);
  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

void ButtonMakerOverlay::keyPressEvent(QKeyEvent *event)
{
  bool ctrlPressed = QApplication::keyboardModifiers() & Qt::ControlModifier;

  if(event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9 && ctrlPressed )
  {
    int keyPressed = event->key() - Qt::Key_0;
    m_presetPositions[keyPressed] = m_rectPos;
    m_presetScales[keyPressed] = m_rectSize;
    return;
  }

  if(event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
  {
    int keyPressed = event->key() - Qt::Key_0;
    if(!m_presetPositions.contains(keyPressed))
      return;
    m_rectPos = m_presetPositions[keyPressed];
    m_rectSize = m_presetScales[keyPressed];
    update();
    return;
  }

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
    case Qt::Key_R:
      if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
        m_rectPos = QPoint((width() / 2) - (m_rectSize / 2), 80);
        m_rectSize = 120;
      }
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
  if(!m_OverlayImage.isNull())
    painter.drawImage(rect, m_OverlayImage);
  if(!m_buttonPreview.isNull())
    painter.drawImage(QRect(0, 0, 82, 82), m_buttonPreview);

}

void ButtonMakerOverlay::mousePressEvent(QMouseEvent *event)
{
  setFocus();
  QRect rect(m_rectPos, QSize(m_rectSize, m_rectSize));
  if(rect.contains(event->pos())) {
    m_dragging = true;
    m_dragOffset = event->pos() - m_rectPos;
  }
}

void ButtonMakerOverlay::mouseMoveEvent(QMouseEvent *event)
{
  if(!m_dragging) return;

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
