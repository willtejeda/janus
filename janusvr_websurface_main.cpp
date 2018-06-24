#include <memory>
#include <QApplication>
#include "offscreenwebsurface.h"

int main(int argc, char *argv[])
{
	QSurfaceFormat default_surface_format;
	default_surface_format.setSamples(4);
	default_surface_format.setDepthBufferSize(24);
	default_surface_format.setStencilBufferSize(8);
	QSurfaceFormat::setDefaultFormat(default_surface_format);
	
	QApplication app(argc, argv);
	app.setStyle(QStyleFactory::create("fusion"));    

    OffScreenWebSurface webSurface;
    webSurface.MainFunction(argc, argv);
//    OffScreenWebSurface* webSurface = new OffScreenWebSurface();
//    webSurface->main_function(argc, argv);

	return app.exec();
}
