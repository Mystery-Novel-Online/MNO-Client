#include "loop_detection.h"
#include "engine/system/audio.h"

#include <QProgressDialog>

constexpr int FFT_SIZE = 2048;
constexpr int FINGERPRINT_BINS = 64;
constexpr int MIN_LOOP_SECONDS = 15;
constexpr float SILENCE_THRESHOLD = 1e-4f;
constexpr float SMOOTH_ALPHA = 0.7f;

constexpr int PIXELS_PER_SECOND = 80;

struct SpectralFrame { std::array<float, FINGERPRINT_BINS> bins{}; };

double streamLength(HSTREAM& stream)
{
  QWORD lengthBytes = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
  if (lengthBytes == (QWORD)-1) {
    return -1;
  }
  return BASS_ChannelBytes2Seconds(stream, lengthBytes);
}

void LoopDetection::FindLoop(QString fileName)
{
  QProgressDialog progress("Finding loop points...", "Cancel", 0, 100);

  progress.setWindowModality(Qt::ApplicationModal);
  progress.setMinimumDuration(0);
  progress.setAutoClose(false);
  progress.setAutoReset(false);
  progress.show();

  HSTREAM stream;

  if (fileName.endsWith("opus", Qt::CaseInsensitive))
  {
    stream = BASS_OPUS_StreamCreateFile(FALSE, fileName.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE | BASS_STREAM_DECODE | BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT);
  }
  else
  {
    stream = BASS_StreamCreateFile(FALSE, fileName.utf16(), 0, 0, BASS_UNICODE | BASS_ASYNCFILE | BASS_STREAM_DECODE | BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT);
  }

  if (!stream) return;

  double lengthSeconds = streamLength(stream);

  if(lengthSeconds == -1)
  {
    BASS_StreamFree(stream);
    return;
  }

  int numFrames = int(lengthSeconds * PIXELS_PER_SECOND);
  if (numFrames <= 0) {
    BASS_StreamFree(stream);
    return;
  }

  std::array<float, FFT_SIZE> fft{};
  std::vector<SpectralFrame> frames; frames.reserve(numFrames);
  std::vector<float> energies; energies.reserve(numFrames);

  for (int frameIdx = 0; frameIdx < numFrames; ++frameIdx)
  {
    double timeSec = double(frameIdx) / double(numFrames) * lengthSeconds;

    QWORD pos = BASS_ChannelSeconds2Bytes(stream, timeSec);
    BASS_ChannelSetPosition(stream, pos, BASS_POS_BYTE);

    int read = BASS_ChannelGetData(stream, fft.data(), BASS_DATA_FFT2048);
    if (read <= 0) continue;

    SpectralFrame frame{};
    float frameEnergy = 0.0f;

    for (int y = 5; y < FFT_SIZE / 2; ++y)
    {
      float mag = fft[y];
      frameEnergy += mag;

      int bin = (y * FINGERPRINT_BINS) / (FFT_SIZE / 2);
      frame.bins[bin] += mag;
    }

    frameEnergy /= (FFT_SIZE / 2);
    energies.push_back(frameEnergy);

    float sum = 0.0f;
    for (float v : frame.bins) sum += v;
    if (sum > 0.0f)
      for (float& v : frame.bins) v /= sum;

    if (!frames.empty()) {
      for (int i = 0; i < FINGERPRINT_BINS; ++i)
        frame.bins[i] = SMOOTH_ALPHA * frame.bins[i] + (1.0f - SMOOTH_ALPHA) * frames.back().bins[i];
    }

    frames.push_back(frame);

    if ((frameIdx & 7) == 0)
    {
      int percent = int(30.0f * frameIdx / float(numFrames));
      progress.setValue(percent);
      QCoreApplication::processEvents();

      if (progress.wasCanceled())
      {
        BASS_StreamFree(stream);
        return;
      }
    }
  }

  int lastAudioFrame = frames.size() - 1;
  while (lastAudioFrame > 0 && energies[lastAudioFrame] < SILENCE_THRESHOLD)
    --lastAudioFrame;

  int minLoopFrames = MIN_LOOP_SECONDS * PIXELS_PER_SECOND;

  float bestScore = std::numeric_limits<float>::max();
  int bestStart = 0, bestEnd = 0;

  for (int start = 0; start < lastAudioFrame - minLoopFrames; ++start)
  {
    if ((start & 15) == 0)
    {
      int percent = 30 + int(70.0f * start / float(lastAudioFrame - minLoopFrames));
      progress.setValue(percent);
      QCoreApplication::processEvents();

      if (progress.wasCanceled())
      {
        BASS_StreamFree(stream);
        return;
      }
    }

    for (int end = start + minLoopFrames; end <= lastAudioFrame; ++end)
    {
      float dist = 0.0f;
      for (int i = 0; i < FINGERPRINT_BINS; ++i)
      {
        float d = frames[start].bins[i] - frames[end].bins[i];
        dist += d * d;
      }

      dist += 0.5f * qAbs(energies[start] - energies[end]);

      dist *= 1.0f - 0.3f * float(end) / lastAudioFrame;

      if (dist < bestScore)
      {
        bestScore = dist;
        bestStart = start;
        bestEnd = end;
      }
    }
  }


  progress.setValue(100);

  double startTimeSec = double(bestStart) / double(numFrames) * lengthSeconds;
  double endTimeSec = double(bestEnd) / double(numFrames) * lengthSeconds;

  QWORD startByte = BASS_ChannelSeconds2Bytes(stream, startTimeSec);
  QWORD endByte   = BASS_ChannelSeconds2Bytes(stream, endTimeSec);

  BASS_CHANNELINFO info{};
  BASS_ChannelGetInfo(stream, &info);
  DWORD bytesPerSampleFrame = ((info.flags & BASS_SAMPLE_FLOAT) ? 4 : 2) * info.chans;

  QWORD startSample = startByte / bytesPerSampleFrame;
  QWORD endSample   = endByte / bytesPerSampleFrame;

  {
    QDialog dlg;
    dlg.setWindowTitle("Loop Found");

    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QLabel* infoLabel = new QLabel(QString("Loop start sample: %1 \nLoop end sample: %2")
        .arg(startSample)
        .arg(endSample)
    );
    infoLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(infoLabel);

    QHBoxLayout* btnLayout = new QHBoxLayout();

    QPushButton* previewBtn = new QPushButton("Preview");
    QPushButton* closeBtn   = new QPushButton("Close");

    btnLayout->addStretch();
    btnLayout->addWidget(previewBtn);
    btnLayout->addWidget(closeBtn);

    layout->addLayout(btnLayout);

    QObject::connect(previewBtn, &QPushButton::clicked, [&]() {
      audio::bgm::PlayDefineLoop(
        fileName.toStdString(),
        startSample,
        endSample
      );
    });

    QObject::connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    dlg.exec();
  }
  qDebug() << "Loop start: pixel" << bestStart << ", time" << startTimeSec << "s, sample" << startSample;
  qDebug() << "Loop end:   pixel" << bestEnd   << ", time" << endTimeSec   << "s, sample" << endSample;

  BASS_StreamFree(stream);
}
