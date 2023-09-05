#include <cmath>
#include <cstdint>

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define HAS_QT6
#endif

#ifdef HAS_QT6
#include <QAudioDevice>
#include <QAudioSink>
#include <QMediaDevices>
#else
#include <QAudioDeviceInfo>
#endif
#include <QAudioOutput>
#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QIODevice>

class SineWaveGenerator : public QIODevice
{
    Q_OBJECT

public:
    SineWaveGenerator(QObject* parent = nullptr) :
        QIODevice(parent),
        sample_rate(44100),
        amplitude(0.5),
        frequency(440.0),
        phase(0.0)
    {
        open(QIODevice::ReadOnly);
    }

    qint64 readData(char *data, qint64 maxlen) {
        const int sample_size = 2;
        const double twopi = 2.0 * M_PI;

        qint64 total = 0;
        while (total < maxlen) {
            double value = amplitude * std::sin(phase);
            std::int16_t sample = static_cast<int16_t>(value * INT16_MAX);

            for (int i = 0; i < sample_size; ++i)
            {
                data[total++] = static_cast<char>((sample >> (8 * i)) & 0xff);
            }

            phase += twopi * frequency / sample_rate;
            if (phase >= twopi) {
                phase -= twopi;
            }
        }

        return total;
    }

    qint64 writeData(const char *, qint64) {
        return 0;
    }

private:
    int sample_rate;
    double amplitude;
    double frequency;
    double phase;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    SineWaveGenerator generator;
    generator.open(QIODevice::ReadOnly);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
#ifdef HAS_QT6
    format.setSampleFormat(QAudioFormat::Int16);
    auto device = QMediaDevices::defaultAudioOutput();
    if (!device.isFormatSupported(format)) {
        qWarning() << "Default audio format not supported - exiting";
        return 1;
    }
    QAudioSink sink(device, format);;
    sink.start(&generator);
#else
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    auto device = QAudioDeviceInfo::defaultOutputDevice();
    if (!device.isFormatSupported(format)) {
        qWarning() << "Default audio format not supported - exiting";
        return 1;
    }
    QAudioOutput audio_output(device, format);
    audio_output.start(&generator);
#endif

    return app.exec();
}

#include "qt-sound.moc"
