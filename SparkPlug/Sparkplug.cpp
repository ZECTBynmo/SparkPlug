#include "Sparkplug.h"
#include "Engine.h"

SparkPlug::SparkPlug(QWidget *parent, Qt::WFlags flags) : 
QMainWindow(parent, flags) {
	
	// Setup the UI using the Qt UI thing
	ui.setupUi(this);
	
	Engine* pEngine= new Engine();
}

SparkPlug::~SparkPlug() {

}
