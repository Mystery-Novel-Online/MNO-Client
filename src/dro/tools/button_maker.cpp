#include "button_maker.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include "aoapplication.h"
#include "dro/fs/fs_reading.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/param/actor_repository.h"
#include "dro/system/text_encoding.h"
#include <QSlider>

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

void ButtonMaker::SetEmote(DREmote emote)
{
  if(m_Emotes.count() == 0) return;

  if(emote.character != m_Emotes.at(0).character)
  {
    hide();
    return;
  }

  for(int i = 0; i < m_Emotes.count(); i++)
  {
    DREmote checkEmote = m_Emotes.at(i);

    if(emote.comment == checkEmote.comment)
    {
      m_EmoteIndex = i;
      m_CharacterSprite->play_idle(checkEmote.character, checkEmote.dialog);
      m_CharacterSprite->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));

      QStringList layers;
      for(const EmoteLayer &layer : checkEmote.emoteOverlays)
      {
        if(dro::actor::user::layerState(layer.toggleName))
          layers.append(dro::system::encoding::text::EncodePacketContents({layer.spriteName, layer.spriteOrder, QString::number(layer.layerOffset.x()), QString::number(layer.layerOffset.y()), QString::number(layer.layerOffset.width()), QString::number(layer.layerOffset.height()), layer.offsetName}));
      }

      m_CharacterSprite->processOverlays(dro::system::encoding::text::EncodeBase64(layers), checkEmote.character, checkEmote.dialog, checkEmote.outfitName);
      m_CharacterSprite->start(DRCharacterMovie::ScalingMode::WidthSmoothScaling, 1.0f);
      return;
    }
  }

}

void ButtonMaker::SetCharacter(QString character)
{
  m_EmoteIndex = 0;
  m_Emotes.clear();

  m_Path = AOApplication::getInstance()->get_character_path(character, "char.json");
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

  if (!m_OverlayImage.isNull())
  {
    QImage overlayScaled = m_OverlayImage.scaled(outputSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    finalPainter.drawImage(0, 0, overlayScaled);
  }

  finalPainter.end();

  QString buttonDirectory = "/emotions/button" + QString::number(m_EmoteIndex + 1) + "_off.png";
  if(m_IsJson)
  {
    buttonDirectory = "/outfits/" + m_Emotes.at(m_EmoteIndex).outfitName + "/emotions/" + m_Emotes.at(m_EmoteIndex).emoteName + ".png";
  }

  QString filePath = AOApplication::getInstance()->get_character_path(m_Emotes.at(m_EmoteIndex).character, buttonDirectory);
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


  m_CharacterSprite->play_idle(m_Emotes.at(m_EmoteIndex).character, m_Emotes.at(m_EmoteIndex).dialog);
  m_CharacterSprite->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));
  m_CharacterSprite->start(DRCharacterMovie::ScalingMode::WidthSmoothScaling, 1.0f);
}

void ButtonMaker::onAddUnderlayClicked()
{
  m_UnderlayImage = LoadImageDialog();
}

void ButtonMaker::onAddOverlayClicked()
{
  m_OverlayImage = LoadImageDialog();
}

void ButtonMaker::onAlphaClicked()
{
  m_AlphaMaskImage = LoadImageDialog();
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
}

void ButtonMakerOverlay::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setPen(Qt::white);
  painter.drawRect(QRect(m_rectPos, QSize(m_rectSize, m_rectSize)));
}

void ButtonMakerOverlay::mousePressEvent(QMouseEvent *event)
{
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
