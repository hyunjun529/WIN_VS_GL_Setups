#include "util/Log.h"

#include "viewer/Viewer.h"


int main(int argc, char** argv)
{
	KATA_CONSOLE_INFO("start");

	kata::Viewer *viewer = new kata::Viewer();

	viewer->addScene(1);
	viewer->addScene(2);
	viewer->addScene(3);
	viewer->addScene(4);

	viewer->run();

	return 0;
}