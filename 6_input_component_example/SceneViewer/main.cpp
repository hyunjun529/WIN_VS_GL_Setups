#include "util/Log.h"

#include "viewer/Viewer.h"


int main(int argc, char** argv)
{
	KATA_CONSOLE_INFO("start");

	kata::Viewer *viewer = new kata::Viewer();

	viewer->addScene(20);
	viewer->addScene(40);

	viewer->run();

	return 0;
}