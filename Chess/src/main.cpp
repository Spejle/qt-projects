#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"
#include "piecerenderer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("ChessQt");
    app.setApplicationDisplayName("Chess Qt");
    app.setOrganizationName("ChessQt");

    // Set a modern style
    if (QStyleFactory::keys().contains("Fusion")) {
        app.setStyle(QStyleFactory::create("Fusion"));
    }

    // Set Application Window Icon dynamically from Knight piece
    QIcon appIcon(PieceRenderer::piecePixmap(PieceType::Knight, PieceColor::White, 64));
    app.setWindowIcon(appIcon);

    MainWindow window;
    window.show();

    return app.exec();
}
