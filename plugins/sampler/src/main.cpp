#include "sampler.h"

// system
#include <stdio.h>
#include <pthread.h>
#include <string>

using namespace std;
using namespace Lumiverse;

int main(int argc, char**argv) {

    // parse commandline input
    if (argc != 2) { cout << "Usage: sampler <path_to_rig>"; }

    // load rig
    Rig rig(argv[1]);

    // Initialize the rig
    rig.init();

    // Start the rig
    rig.run();

    // sample each device in the rig
    sample_rig(rig, PANEL);

    // hold
    getchar();

    // reset on exit
    rig.resetDevices();

}


void sample_devices(DeviceSet device_set, SampleMode mode) {

    set<Device*>devices = device_set.getDevices();
    Device *device;

    switch(mode) {
        case PANEL:

            // turn on all devices in the panel
            for (set<Device*>::iterator d = devices.begin();
                                        d != devices.end(); ++d) {

                device = (Device*) *d;

                // turn on decice and set to white
                device->getColor()->setRGBRaw(1,1,1,1);
            }

            // hold till shutter releasd
            getchar();

            // turn off panel
            for (set<Device*>::iterator d = devices.begin(); d != devices.end(); ++d) {

                device = (Device*) *d;

                // turn off device
                device->getColor()->setWeight(0);
            }

            break;

        case DEVICE:

            // turn on devices one at a time
            for (set<Device*>::iterator d = devices.begin();
                                        d != devices.end(); ++d) {

                device = (Device*) *d;

                // turn on decice and set to white
                device->getColor()->setRGB(1,1,1,1);

                // hold till shutter released
                sleep(1);

                // turn off device
                device->getColor()->setWeight(0);

            }

            break;

        default:
            return;
    }

}

void sample_rig(Rig rig, SampleMode mode) {

    set<Device*>devices = rig.getAllDevices().getDevices();

    // turn off all devices and record all panels
    Device *device;
    std::set<int> ids;
    for (set<Device*>::iterator d = devices.begin(); d != devices.end(); ++d) {

        device = (Device*) *d;

        // turn off device
        device->getColor()->setWeight(0);

        // get all panel id's
        ids.insert(stoi(device->getMetadata("panel")));
    }

    // sample each panel
    set<int>::iterator id = ids.begin();
    advance(id,0);
    while (id != ids.end()) {

        // select matching device
        sample_devices(rig.getDevices("panel", to_string(*id), true), mode);
        ++id;
    }
}
