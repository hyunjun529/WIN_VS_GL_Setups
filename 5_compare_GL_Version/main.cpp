#include "util/Log.h"

#include "viewer/Viewer.h"


int main(int argc, char** argv)
{
	CONSOLE_INFO("start");

	kata::Viewer *viewer = new kata::Viewer();

	viewer->addScene();

	viewer->run();

	return 0;
}