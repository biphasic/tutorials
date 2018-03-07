#include <sepia.hpp>
#include <tarsier/stitch.hpp>
#include <chameleon/backgroundCleaner.hpp>
#include <chameleon/changeDetectionDisplay.hpp>
#include <chameleon/logarithmicDisplay.hpp>
#include <chameleon/frameGenerator.hpp>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtCore/QFileInfo>
#include <QtGui/QVector2D>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

/// ExposureMeasurement holds the parameters of an exposure measurement.
struct ExposureMeasurement {
    size_t x;
    size_t y;
    uint64_t timeDelta;
};

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<chameleon::BackgroundCleaner>("BackgroundCleaner", 1, 0, "BackgroundCleaner");
    //qmlRegisterType<chameleon::ChangeDetectionDisplay>("ChangeDetectionDisplay", 1, 0, "ChangeDetectionDisplay");
    qmlRegisterType<chameleon::LogarithmicDisplay>("LogarithmicDisplay", 1, 0, "LogarithmicDisplay");
    qmlRegisterType<chameleon::FrameGenerator>("FrameGenerator", 1, 0, "FrameGenerator");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QQuickView view(QUrl::fromLocalFile(QFileInfo("../source/main.qml").absoluteFilePath()));
    view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    //auto changeDetectionDisplay = view.rootObject()->findChild<chameleon::ChangeDetectionDisplay*>("changeDetectionDisplay");
    auto logarithmicDisplay = view.rootObject()->findChild<chameleon::LogarithmicDisplay*>("logarithmicDisplay");
    auto frameGenerator = view.rootObject()->findChild<chameleon::FrameGenerator*>("frameGenerator");

    uint64_t firstTimestamp = 0;
    auto firstTimestampSet = false;
    std::size_t frameIndex = 0;
    //QVector2D qVector(2000,150000);
    QVector2D qVector;
    std::cout << "*argv[3]: " << *argv[3] << '\n';

    istringstream ss(argv[3]);
    int x;
    if(!(ss >> x))
      cerr << " Invalid number " << argv[3] << '\n';

    istringstream tt(argv[4]);
    int y;
    if(!(tt >> y))
      cerr << " Invalid number " << argv[4] << '\n';

    qVector[0] = static_cast<float>(x);
    qVector[1] = static_cast<float>(y);
    std::string input = "/Users/gregorlenz/Nextcloud/face-detection/";
    input.append(argv[1]);
    input.append("/");
    std::string output = input;
    output.append("frames/");
    input.append(argv[2]);

    std::cout << "input: " << input << ", output: " << output << '\n';

    auto atisEventStreamObservable = sepia::make_atisEventStreamObservable(
         input,
        sepia::make_split(
            [&](sepia::DvsEvent dvsEvent) -> void {
                if (!firstTimestampSet) {
                    firstTimestampSet = true;
                    firstTimestamp = dvsEvent.timestamp;
                    logarithmicDisplay->setDiscards(qVector);
                }
                if (dvsEvent.timestamp - firstTimestamp >= 20000 * frameIndex) {
                    frameGenerator->saveFrameTo(output + std::to_string(frameIndex) + ".png");
                    const auto discards = logarithmicDisplay->discards();
                    std::cout << "Frame " << frameIndex << ": black discard: " << discards.x() << ", " << "whiteDiscard: " << discards.y() << std::endl;
                    ++frameIndex;
                }
                //changeDetectionDisplay->push(dvsEvent);
            },
            tarsier::make_stitch<sepia::ThresholdCrossing, ExposureMeasurement, 304, 240>(
                [](sepia::ThresholdCrossing secondThresholdCrossing, uint64_t timeDelta) -> ExposureMeasurement {
                    return ExposureMeasurement{secondThresholdCrossing.x, secondThresholdCrossing.y, timeDelta};
                },
                [&](ExposureMeasurement exposureMeasurement) -> void {
                    logarithmicDisplay->push(exposureMeasurement);
                }
            )
        ),
        [](std::exception_ptr) {}
    );

    return app.exec();
}
