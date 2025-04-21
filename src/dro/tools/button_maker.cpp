#include "button_maker.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include "aoapplication.h"
#include "file_functions.h"
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
  rightLayout->addWidget(generateButton);
  connect(generateButton, &QPushButton::clicked, this, &ButtonMaker::onGenerateClicked);

  mainLayout->addLayout(leftLayout);
  mainLayout->addStretch();
  mainLayout->addLayout(rightLayout);

  setLayout(mainLayout);
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

    if(emote.dialog == checkEmote.dialog)
    {
      m_EmoteIndex = i;
      m_CharacterSprite->play_idle(m_Emotes.at(m_EmoteIndex).character, m_Emotes.at(m_EmoteIndex).dialog);
      return;
    }
  }
}

void ButtonMaker::SetCharacter(QString character)
{
  m_EmoteIndex = 0;
  m_Emotes.clear();

  m_Path = AOApplication::getInstance()->get_character_path(character, "char.json");
  if(file_exists(m_Path))
  {
    m_IsJson = true;
    ActorDataReader jsonActor = ActorDataReader();
    jsonActor.loadActor(character);
    jsonActor.SetOutfit("<All>");
    m_Emotes = jsonActor.getEmotes();
  }
  else
  {
    m_IsJson = false;
    LegacyActorReader legacyActor = LegacyActorReader();
    legacyActor.loadActor(character);
    m_Emotes = legacyActor.getEmotes();
  }

  m_CharacterSprite->play_idle(m_Emotes.at(m_EmoteIndex).character, m_Emotes.at(m_EmoteIndex).dialog);
}

void ButtonMaker::onGenerateClicked()
{
  QImage fullImage(m_GraphicsView->viewport()->size(), QImage::Format_ARGB32_Premultiplied);
  fullImage.fill(Qt::transparent);

  QPainter painter(&fullImage);
  m_GraphicsView->render(&painter);
  painter.end();

  QRect cropRect(m_Overlay->m_rectPos, QSize(m_Overlay->m_rectSize, m_Overlay->m_rectSize));
  cropRect = cropRect.intersected(fullImage.rect());

  QImage cropped = fullImage.copy(cropRect);

  QString buttonDirectory = "/emotions/button" + QString::number(m_EmoteIndex + 1) + "_off.png";
  if(m_IsJson)
  {
    buttonDirectory = "/outfits/" + m_Emotes.at(m_EmoteIndex).outfitName + "/emotions/" + m_Emotes.at(m_EmoteIndex).emoteName + ".png";
  }

  QString filePath = AOApplication::getInstance()->get_character_path(m_Emotes.at(m_EmoteIndex).character, buttonDirectory);
  if(file_exists(filePath))
  {
    QMessageBox::StandardButton replaceResult;
    replaceResult = QMessageBox::question(nullptr, "Button Maker", "You are about to replace a button that already exists, continue?", QMessageBox::Yes|QMessageBox::No);
    if(replaceResult == QMessageBox::No)
    {
      return;
    }

  }
  if(!dir_exists(QFileInfo(filePath).absolutePath()))
  {
    QDir().mkdir(QFileInfo(filePath).absolutePath());
  }
  cropped.save(filePath);

  if(m_EmoteIndex < (m_Emotes.count() - 1))
  {
    m_EmoteIndex++;
  }
  else
  {
    m_EmoteIndex = 0;
  }


  m_CharacterSprite->play_idle(m_Emotes.at(m_EmoteIndex).character, m_Emotes.at(m_EmoteIndex).dialog);
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
